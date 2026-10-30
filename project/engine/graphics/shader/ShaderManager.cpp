#include "ShaderManager.h"
#include "ShaderCompiler.h"
using namespace Microsoft::WRL;

//生成
std::unique_ptr<ShaderManager> ShaderManager::Create(ConstructorKey key){
	std::unique_ptr<ShaderManager>instance = std::make_unique<ShaderManager>(key);
	instance->Initialize();
	return instance;
}

//コンストラクタ
ShaderManager::ShaderManager(ConstructorKey){
}

//デストラクタ
ShaderManager::~ShaderManager(){
}

//初期化
void ShaderManager::Initialize(){
	//シェーダコンパイラの生成
	shaderCompiler_ = ShaderCompiler::Create();
}

//シェーダの情報を取得
ShaderInfo* ShaderManager::FindShaderInfo(const std::string& name, const std::wstring& filePath, const wchar_t* profile){
	//存在していたら
	if (shaderInfos_.contains(name)){
		return &shaderInfos_.at(name);
	}

	//パスが存在しなければ
	if (filePath.empty()){
		return nullptr;
	}
	//ディレクトリパス
	std::wstring directoryPath = L"engine/resources/shaders/pixels/";

	//存在していない場合
	//コンパイルを行う
	shaderCompiler_->CompileShader(directoryPath + filePath, profile);
	//コンパイルしたシェーダにアクセス
	std::unordered_map<std::string, ParameterInfo>parameters;
	for (const CompileShaderInfo& compileShaderInfo : shaderCompiler_->GetCompileShaderInfos()){
		//変数名
		std::string constantBufferName = compileShaderInfo.name;
		//パラメータの情報
		ParameterInfo parameterInfo = {
			compileShaderInfo.offset,
			compileShaderInfo.size
		};
		//パラメータに追加
		parameters.insert(std::make_pair(constantBufferName, parameterInfo));
	}
	//ShaderInfoに追加
	const ComPtr<IDxcBlob>& shaderBlob = shaderCompiler_->GetShaderBlob();
	ShaderInfo shaderInfo = {
		shaderBlob,
		parameters
	};
	shaderInfos_.insert(std::make_pair(name, shaderInfo));
	return &shaderInfos_.at(name);
}
