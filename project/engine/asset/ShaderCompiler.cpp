#include "ShaderCompiler.h"
#include "Logger.h"
#include "StringUtility.h"
#include <d3d12shader.h>
#include <cassert>
#pragma comment(lib,"dxcompiler.lib")
using namespace Microsoft::WRL;

//生成
std::unique_ptr<ShaderCompiler> ShaderCompiler::Create(){
	std::unique_ptr<ShaderCompiler>instance = std::make_unique<ShaderCompiler>();
	instance->Initialize();
	return instance;
}

//コンストラクタ
ShaderCompiler::ShaderCompiler(){
}

//デストラクタ
ShaderCompiler::~ShaderCompiler(){
}

//初期化
void ShaderCompiler::Initialize(){
	//DXCコンパイラの生成
	CreateDXCCompiler();
}

//コンパイルシェーダ
ComPtr<IDxcBlob> ShaderCompiler::CompileShader(const std::wstring& filePath, const wchar_t* profile){
	//1. hlslファイルを読み込む
	//これからシェーダーをコンパイルする旨をログに出す
	Logger::OutputLog(stringUtility::ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));
	//hlslファイルを読み込む
	ComPtr<IDxcBlobEncoding> shaderSource = nullptr;
	HRESULT hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読み込めなかったら止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;//UTF8の文字コードであることを通知
	//2. Compilerする
	LPCWSTR arguments[] = {
		filePath.c_str(),//コンパイル対象のhlslファイル名
		L"-I", L"engine/resources/shaders/header",
		L"-E",L"main",//エントリーポイントの指定。基本的にmain以外にはしない
		L"-T",profile,//ShaderProfileの設定
		L"-Zi",L"-Qembed_debug",//デバック用の情報を埋め込む
		L"-Od",//最適化を外しておく
		L"-Zpr",//メモリレイアウトは行優先
	};
	//実際にShaderをコンパイルする
	ComPtr<IDxcResult> shaderResult = nullptr;
	hr = dxcCompiler_->Compile(
		&shaderSourceBuffer,//読み込んだファイル
		arguments,//コンパイルオプション
		_countof(arguments),//コンパイルオプションの数
		includeHandler_.Get(),//includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)//コンパイル結果
	);
	//コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));
	//3. 警告・エラーが出ていないか確認する
	ComPtr<IDxcBlobUtf8> shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0){
		Logger::OutputLog(shaderError->GetStringPointer());
		//警告・エラーダメゼッタイ
		assert(false);
	}
	//4. Compiler結果を受け取って返す
	//コンパイル結果から実行用のバイナリ部分を取得
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob_), nullptr);
	assert(SUCCEEDED(hr));
	//成功したらログを出す
	Logger::OutputLog(stringUtility::ConvertString(std::format(L"Compile Succeeded, path:{},profile:{}\n", filePath, profile)));
	//シェーダリフレクション
	ReflectionShader(shaderResult);

	//実行用のバイナリを返却
	return shaderBlob_;
}

//コンパイル済みのシェーダBlobを取得
const ComPtr<IDxcBlob>& ShaderCompiler::GetShaderBlob() const{
	return shaderBlob_;
}

//コンパイルした際のシェーダ情報を取得
const std::vector<CompileShaderInfo>& ShaderCompiler::GetCompileShaderInfos() const{
	return compileShaderInfos_;
}

//コンパイルしたシェーダの情報の数の取得
uint32_t ShaderCompiler::GetCompileShaderInfoCount() const{
	return static_cast<uint32_t>(compileShaderInfos_.size());
}

//DXCコンパイラの生成
void ShaderCompiler::CreateDXCCompiler(){
	HRESULT result = S_FALSE;
	//DXCユーティリティの生成
	result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(result));
	//DXCコンパイラの生成
	result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(result));
	//デフォルトインクルードハンドラの生成
	result = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(result));
}

//シェーダリフレクション
void ShaderCompiler::ReflectionShader(ComPtr<IDxcResult> shaderResult){
	HRESULT hr = S_FALSE;
	//Reflectionデータの取得
	ComPtr<IDxcBlob>reflectionBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&reflectionBlob), nullptr);
	assert(SUCCEEDED(hr));
	//IDxcUtilsでReflectionを作成
	DxcBuffer reflectionData{};
	reflectionData.Ptr = reflectionBlob->GetBufferPointer();
	reflectionData.Size = reflectionBlob->GetBufferSize();
	reflectionData.Encoding = 0;

	ComPtr<ID3D12ShaderReflection> reflection = nullptr;
	hr = dxcUtils_->CreateReflection(&reflectionData, IID_PPV_ARGS(&reflection));

	//CBufferの変数にアクセス
	D3D12_SHADER_DESC shaderDesc = {};
	reflection->GetDesc(&shaderDesc);
	for (uint32_t i = 0; i < shaderDesc.ConstantBuffers; i++){
		//最初のCBuffer
		auto* constantBuffer = reflection->GetConstantBufferByIndex(i);

		//最初の変数
		auto* variable = constantBuffer->GetVariableByIndex(i);

		D3D12_SHADER_VARIABLE_DESC variableDesc = {};
		variable->GetDesc(&variableDesc);
		//値を追加
		std::string name = static_cast<std::string>(variableDesc.Name);
		uint32_t offset = static_cast<uint32_t>(variableDesc.StartOffset);
		uint32_t size = static_cast<uint32_t>(variableDesc.Size);
		compileShaderInfos_.push_back({ name,offset,size });
	}
}
