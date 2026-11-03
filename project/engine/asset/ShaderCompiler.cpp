#include "ShaderCompiler.h"
#include "Logger.h"
#include "StringUtility.h"
#include <cassert>
#include <array>
#pragma comment(lib,"dxcompiler.lib")
using namespace Microsoft::WRL;

//生成
std::unique_ptr<ShaderCompiler> ShaderCompiler::Create() {
	std::unique_ptr<ShaderCompiler>instance = std::make_unique<ShaderCompiler>();
	instance->Initialize();
	return instance;
}

//コンストラクタ
ShaderCompiler::ShaderCompiler() {
}

//デストラクタ
ShaderCompiler::~ShaderCompiler() {
}

//初期化
void ShaderCompiler::Initialize() {
	//DXCコンパイラの生成
	CreateDXCCompiler();
}

//コンパイルシェーダ
void ShaderCompiler::CompileShader(const std::wstring& filePath , const wchar_t* profile) {
	//1. hlslファイルを読み込む
	//これからシェーダーをコンパイルする旨をログに出す
	Logger::OutputLog(stringUtility::ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n" , filePath , profile)));
	//hlslファイルを読み込む
	ComPtr<IDxcBlobEncoding> shaderSource = nullptr;
	HRESULT hr = dxcUtils_->LoadFile(filePath.c_str() , nullptr , &shaderSource);
	//読み込めなかったら止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;//UTF8の文字コードであることを通知
	//2. Compilerする
	LPCWSTR arguments[] = {
		filePath.c_str() ,//コンパイル対象のhlslファイル名
		L"-I" , L"engine/resources/shaders/headers" ,
		L"-E" , L"main" ,//エントリーポイントの指定。基本的にmain以外にはしない
		L"-T" , profile ,//ShaderProfileの設定
		L"-Zi" , L"-Qembed_debug" ,//デバック用の情報を埋め込む
		L"-Od" ,//最適化を外しておく
		L"-Zpr" ,//メモリレイアウトは行優先
	};
	//実際にShaderをコンパイルする
	ComPtr<IDxcResult> shaderResult = nullptr;
	hr = dxcCompiler_->Compile(
		&shaderSourceBuffer ,//読み込んだファイル
		arguments ,//コンパイルオプション
		_countof(arguments) ,//コンパイルオプションの数
		includeHandler_.Get() ,//includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)//コンパイル結果
	);
	//コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));
	//3. 警告・エラーが出ていないか確認する
	ComPtr<IDxcBlobUtf8> shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS , IID_PPV_ARGS(&shaderError) , nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Logger::OutputLog(shaderError->GetStringPointer());
		//警告・エラーダメゼッタイ
		assert(false);
	}
	//4. Compiler結果を受け取って返す
	//コンパイル結果から実行用のバイナリ部分を取得
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT , IID_PPV_ARGS(&shaderInfo_.shaderBlob) , nullptr);
	assert(SUCCEEDED(hr));
	//成功したらログを出す
	Logger::OutputLog(stringUtility::ConvertString(std::format(L"Compile Succeeded, path:{},profile:{}\n" , filePath , profile)));
	//シェーダリフレクション
	ReflectionShader(shaderResult);
}

//シェーダの情報を取得
const ShaderInfo& ShaderCompiler::GetShaderInfo() const {
	return shaderInfo_;
}

//DXCコンパイラの生成
void ShaderCompiler::CreateDXCCompiler() {
	HRESULT result = S_FALSE;
	//DXCユーティリティの生成
	result = DxcCreateInstance(CLSID_DxcUtils , IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(result));
	//DXCコンパイラの生成
	result = DxcCreateInstance(CLSID_DxcCompiler , IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(result));
	//デフォルトインクルードハンドラの生成
	result = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(result));
}

//シェーダリフレクション
void ShaderCompiler::ReflectionShader(ComPtr<IDxcResult> shaderResult) {
	HRESULT hr = S_FALSE;
	//Reflectionデータの取得
	ComPtr<IDxcBlob>reflectionBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_REFLECTION , IID_PPV_ARGS(&reflectionBlob) , nullptr);
	assert(SUCCEEDED(hr));
	//IDxcUtilsでReflectionを作成
	DxcBuffer reflectionData{};
	reflectionData.Ptr = reflectionBlob->GetBufferPointer();
	reflectionData.Size = reflectionBlob->GetBufferSize();
	reflectionData.Encoding = 0;

	ComPtr<ID3D12ShaderReflection> reflection = nullptr;
	hr = dxcUtils_->CreateReflection(&reflectionData , IID_PPV_ARGS(&reflection));
	assert(SUCCEEDED(hr));

	//CBufferの変数にアクセス
	D3D12_SHADER_DESC shaderDesc = {};
	hr = reflection->GetDesc(&shaderDesc);
	assert(SUCCEEDED(hr));
	for (uint32_t i = 0; i < shaderDesc.ConstantBuffers; i++) {
		//CBufferを取得
		auto* constantBuffer = reflection->GetConstantBufferByIndex(i);

		//CBufferに割り当てられている変数を取得
		D3D12_SHADER_BUFFER_DESC bufferDesc = {};
		hr = constantBuffer->GetDesc(&bufferDesc);
		assert(SUCCEEDED(hr));
		//成功しなかったら
		if (FAILED(hr)) {
			Logger::OutputLog("CBufferに割り当てられている変数を取得できませんでした");
			continue;
		}
		if (bufferDesc.Name == nullptr) {
			Logger::OutputLog("CBufferに割り当てられている変数を取得できませんでした");
			continue;
		}
		//コンスタントバッファの名前
		std::string cBufferName = static_cast<std::string>(bufferDesc.Name);
		//コンスタントバッファの情報
		ConstantBufferInfo cBufferInfo_ = {};
		cBufferInfo_.bufferSize = bufferDesc.Size;
		//レジスタ番号を取得
		D3D12_SHADER_INPUT_BIND_DESC bindingDesc = {};
		hr = reflection->GetResourceBindingDescByName(bufferDesc.Name , &bindingDesc);
		assert(SUCCEEDED(hr));
		cBufferInfo_.registerNum = bindingDesc.BindPoint;

		for (uint32_t j = 0; j < bufferDesc.Variables; j++) {
			ID3D12ShaderReflectionVariable* variable = constantBuffer->GetVariableByIndex(j);

			//型を取得
			ID3D12ShaderReflectionType* reflectionType = variable->GetType();
			D3D12_SHADER_TYPE_DESC typeDesc = {};
			hr = reflectionType->GetDesc(&typeDesc);
			assert(SUCCEEDED(hr));

			D3D12_SHADER_VARIABLE_DESC variableDesc = {};
			hr = variable->GetDesc(&variableDesc);
			assert(SUCCEEDED(hr));

			//メンバ変数を取得
			for (uint32_t k = 0; k < typeDesc.Members; k++) {
				ID3D12ShaderReflectionType* memberType = reflectionType->GetMemberTypeByIndex(k);
				D3D12_SHADER_TYPE_DESC memberDesc = {};
				memberType->GetDesc(&memberDesc);
				//値を追加
				std::string name = static_cast<std::string>(reflectionType->GetMemberTypeName(k));
				ParameterType type = ConvertToParameterType(memberDesc);
				uint32_t offset = static_cast<uint32_t>(variableDesc.StartOffset + memberDesc.Offset);
				uint32_t size = 0;
				switch (memberDesc.Class) {
					case D3D_SVC_SCALAR: {
						size = 4;
						break;
					}
					case D3D_SVC_VECTOR: {
						size = memberDesc.Columns * 4;
						break;
					}
					case D3D_SVC_MATRIX_ROWS: {
						size = memberDesc.Rows * 16;
						break;
					}
				}
				if (!cBufferInfo_.parameters.contains(name)) {
					ParameterInfo parameter = { type , offset , size };
					cBufferInfo_.parameters.insert(std::make_pair(name , parameter));
				}
			}
		}
		//シェーダの情報に追加
		shaderInfo_.constantBuffers.insert(std::make_pair(cBufferName , cBufferInfo_));
	}
}

//パラメータの型をDirectXの型からわかりやすいように変換
ParameterType ShaderCompiler::ConvertToParameterType(const D3D12_SHADER_TYPE_DESC& desc) {
	//クラス
	D3D_SHADER_VARIABLE_CLASS classType = desc.Class;
	//型
	D3D_SHADER_VARIABLE_TYPE type = desc.Type;
	//列
	uint32_t col = static_cast<UINT>(desc.Columns);

	//パラメータタイプ
	ParameterType parameterType = ParameterType::kUnknow;

	//クラスを確認
	switch (classType) {
		case D3D_SVC_MATRIX_ROWS: {
			if (desc.Type == D3D_SVT_FLOAT) {
				if (desc.Rows == 4 && desc.Columns == 4) {
					parameterType = ParameterType::kMatrix4x4;
				}
			} else {
				parameterType = ParameterType::kUnknow;
			}
			break;
		}
		default: {
			//見やすい方へ変換
			switch (type) {
				case D3D_SVT_FLOAT: {
					//浮動小数点の一覧
					constexpr std::array<ParameterType , 4>floatTypes{
						ParameterType::kFloat ,
						ParameterType::kVector2 ,
						ParameterType::kVector3 ,
						ParameterType::kColor ,
					};
					parameterType = floatTypes[col - 1];
					break;
				}
				case D3D_SVT_INT: {
					parameterType = ParameterType::kInt;
					break;
				}
				case D3D_SVT_UINT: {
					parameterType = ParameterType::kUint;
					break;
				}
			}
			break;
		}
	}
	return parameterType;
}
