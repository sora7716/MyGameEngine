#include "MaterialInstance.h"
#include "MatrixUtility.h"
#include "ShaderManager.h"
#include "StringUtility.h"
#include "Logger.h"
#include <algorithm>

//コンストラクタ
MaterialInstance::MaterialInstance() {
}

//デストラクタ
MaterialInstance::~MaterialInstance() {
}

//初期化
void MaterialInstance::Initialize(const MaterialTexturePaths& texturePaths , ShaderManager* shaderManager) {
	//シェーダの管理を記録
	assert(shaderManager);
	shaderManager_ = shaderManager;

	//テクスチャファイルパスを保存
	texturePaths_ = texturePaths;

	//リムライトの初期化
	rimLight_.color = Color::GetWhite();
	rimLight_.power = 0.1f;
	rimLight_.outLinePower = 0.1f;
	rimLight_.softness = 5.0f;
	rimLight_.enableRimLighting = 0;

	//マテリアルが変更されたかを判断する変数を初期化
	revision_ = 0;
}

//テクスチャの設定
void MaterialInstance::SetTexture(const std::string& path) {
	texturePaths_.textureFilePath = path;

	//変更があったので加算
	revision_++;
}

//環境マップの設定
void MaterialInstance::SetEnvironmentMap(const std::string& path) {
	texturePaths_.environmentMap = path;

	//変更があったので加算
	revision_++;
}

//リムライトの色の設定
void MaterialInstance::SetRimColor(const Color& color) {
	rimLight_.color = color;

	//変更があったので加算
	revision_++;
}

//リムライトの強さの設定
void MaterialInstance::SetRimPower(float power) {
	rimLight_.power = power;

	//変更があったので加算
	revision_++;
}

//リムライトのアウトラインの強さの設定
void MaterialInstance::SetRimOutLinePower(float outLinePower) {
	rimLight_.outLinePower = outLinePower;

	//変更があったので加算
	revision_++;
}

//リムライトの柔らかさの設定
void MaterialInstance::SetRimSoftness(float softness) {
	rimLight_.softness = softness;

	//変更があったので加算
	revision_++;
}

//リムライトをするかの設定
void MaterialInstance::SetIsRimLighting(bool enabled) {
	rimLight_.enableRimLighting = static_cast<int32_t>( enabled );

	//変更があったので加算
	revision_++;
}

//リムライトの設定
void MaterialInstance::SetRimLight(const RimLight& rimLight) {
	rimLight_ = rimLight;

	//変更があったので加算
	revision_++;
}

//リムライトを取得
const RimLight& MaterialInstance::GetRimLight() const {
	return rimLight_;
}

//ピクセルシェーダのファイルパスを設定
void MaterialInstance::SetPixelShaderFileName(const std::string& pixelShaderFileName , const std::string& cBufferName) {
	//シェーダの情報を取得
	ShaderInfo* shaderInfo = shaderManager_->FindPixelShaderInfo(pixelShaderFileName , stringUtility::ConvertString(pixelShaderFileName));
	//シェーダの情報が無ければ
	if (!shaderInfo) {
		return;
	}

	//コンスタントバッファが存在しなかったら
	if (!shaderInfo->constantBuffers.contains(cBufferName)) {
		Logger::OutputLog("コンスタントバッファ名が存在しませんでした");
		return;
	}
	//コンスタントバッファの情報を取得
	cBuffer_ = shaderInfo->constantBuffers.at(cBufferName);

	//ファイル名を取得
	pixelShaderFileName_ = pixelShaderFileName;

	//パラメータのメモリの値の配列サイズを設定
	parameterValues_.clear();
	parameterValues_.resize(cBuffer_.bufferSize);

	//パラメータを保存
	parameterInfos_ = shaderInfo->constantBuffers.at(cBufferName).parameters;

	//変更があったので加算
	revision_++;
}

//マテリアルが変更されたかを判断する変数の取得
uint64_t MaterialInstance::GetRevision() const {
	return revision_;
}

//コンスタントバッファの情報の取得
const ConstantBufferInfo& MaterialInstance::GetCBufferInfo() const {
	return cBuffer_;
}

//パラメータのメモリの値を取得
const std::vector<std::byte>& MaterialInstance::GetParameterValues() const {
	return parameterValues_;
}

//テクスチャファイルパスの取得
const MaterialTexturePaths& MaterialInstance::GetTextureFilePaths() const {
	return texturePaths_;
}
