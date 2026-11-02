#include "ShaderManager.h"
#include "ShaderCompiler.h"
using namespace Microsoft::WRL;

//生成
std::unique_ptr<ShaderManager> ShaderManager::Create(ConstructorKey key) {
	std::unique_ptr<ShaderManager>instance = std::make_unique<ShaderManager>(key);
	instance->Initialize();
	return instance;
}

//コンストラクタ
ShaderManager::ShaderManager(ConstructorKey) {
}

//デストラクタ
ShaderManager::~ShaderManager() {
}

//初期化
void ShaderManager::Initialize() {
	//シェーダコンパイラの生成
	shaderCompiler_ = ShaderCompiler::Create();
}

//シェーダの情報を取得
ShaderInfo* ShaderManager::FindPixelShaderInfo(const std::string& name , const std::wstring& fileName , const wchar_t* profile) {
	//存在していたら
	if (shaderInfos_.contains(name)) {
		return &shaderInfos_.at(name);
	}

	//パスが存在しなければ
	if (fileName.empty()) {
		return nullptr;
	}
	//ディレクトリパス
	std::wstring directoryPath = L"engine/resources/shaders/pixels/";

	//存在していない場合
	//コンパイルを行う
	shaderCompiler_->CompileShader(directoryPath + fileName , profile);
	shaderInfos_.insert(std::make_pair(name , shaderCompiler_->GetShaderInfo()));
	return &shaderInfos_.at(name);
}
