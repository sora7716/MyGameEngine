#include "MaterialInstance.h"
#include "MatrixUtility.h"
#include <algorithm>

//コンストラクタ
MaterialInstance::MaterialInstance(){
}

//デストラクタ
MaterialInstance::~MaterialInstance(){
}

//初期化
void MaterialInstance::Initialize(const std::vector<MaterialTexturePaths>& texturePaths){
	//テクスチャパスと同じ数のスロットを作る
	slots_.assign(texturePaths.size(), MaterialInstanceSlot{});

	for (uint32_t i = 0; i < static_cast<uint32_t>(slots_.size()); i++){
		MaterialInstanceSlot& slot = slots_[i];
		//マテリアルの初期化
		slot.material.color = Vector4::MakeWhiteColor();
		slot.material.enableLighting = true;
		slot.material.environmentCoefficient = 0.0f;
		slot.material.shininess = 10.0f;
		slot.material.uvMatrix = Matrix4x4::Identity4x4();

		//UVトランスフォームの初期化
		slot.uvTransform.scale = Vector2::MakeAllOne();
		slot.uvTransform.rotate = 0.0f;
		slot.uvTransform.translate = { 0.0f,0.0f };

		//テクスチャファイルパスの初期化
		slot.texturePaths = texturePaths[i];
	}

	//リムライトの初期化
	rimLight_.color = Vector4::MakeWhiteColor();
	rimLight_.power = 0.1f;
	rimLight_.outLinePower = 0.1f;
	rimLight_.softness = 5.0f;
	rimLight_.enableRimLighting = 0;

	//マテリアルが変更されたかを判断する変数を初期化
	revision_ = 0;
}

//色の設定
void MaterialInstance::SetColor(uint32_t index, const Vector4& color){
	//検索キーとスロットのサイズを比較して
	if (index >= slots_.size()){
		return;
	}

	slots_[index].material.color = color;

	//変更があったので加算
	revision_++;
}

//テクスチャの設定
void MaterialInstance::SetTexture(uint32_t index, const std::string& path){
	//検索キーとスロットのサイズを比較して
	if (index >= slots_.size()){
		return;
	}

	slots_[index].texturePaths.textureFilePath = path;

	//変更があったので加算
	revision_++;
}

//環境マップの設定
void MaterialInstance::SetEnvironmentMap(uint32_t index, const std::string& path){
	//検索キーとスロットのサイズを比較して
	if (index >= slots_.size()){
		return;
	}

	slots_[index].texturePaths.environmentMap = path;

	//変更があったので加算
	revision_++;
}

//UVのスケールの設定
void MaterialInstance::SetUVScale(uint32_t index, const Vector2& scale){
	//検索キーとスロットのサイズを比較して
	if (index >= slots_.size()){
		return;
	}

	slots_[index].uvTransform.scale = scale;

	slots_[index].material.uvMatrix = matrixUtility::MakeAffineMatrix(slots_[index].uvTransform);

	//変更があったので加算
	revision_++;
}

//UVの回転の設定
void MaterialInstance::SetUVRotate(uint32_t index, float rotate){
	//検索キーとスロットのサイズを比較して
	if (index >= slots_.size()){
		return;
	}

	slots_[index].uvTransform.rotate = rotate;

	slots_[index].material.uvMatrix = matrixUtility::MakeAffineMatrix(slots_[index].uvTransform);

	//変更があったので加算
	revision_++;
}

//UVの平行移動の設定
void MaterialInstance::SetUVTranslate(uint32_t index, const Vector2& translate){
	//検索キーとスロットのサイズを比較して
	if (index >= slots_.size()){
		return;
	}

	slots_[index].uvTransform.translate = translate;

	slots_[index].material.uvMatrix = matrixUtility::MakeAffineMatrix(slots_[index].uvTransform);

	//変更があったので加算
	revision_++;
}

//UVトランスフォームの設定
void MaterialInstance::SetUVTransform(uint32_t index, const RectTransform& transform){
	//検索キーとスロットのサイズを比較して
	if (index >= slots_.size()){
		return;
	}

	slots_[index].uvTransform = transform;

	slots_[index].material.uvMatrix = matrixUtility::MakeAffineMatrix(slots_[index].uvTransform);

	//変更があったので加算
	revision_++;
}

//ライティングするかの設定
void MaterialInstance::SetIsLighting(uint32_t index, bool enabled){
	//検索キーとスロットのサイズを比較して
	if (index >= slots_.size()){
		return;
	}

	slots_[index].material.enableLighting = static_cast<int32_t>(enabled);

	//変更があったので加算
	revision_++;
}

//光沢度の設定
void MaterialInstance::SetShininess(uint32_t index, float shininess){
	//検索キーとスロットのサイズを比較して
	if (index >= slots_.size()){
		return;
	}

	slots_[index].material.shininess = shininess;

	//変更があったので加算
	revision_++;
}

//環境マップの映り込み度の設定
void MaterialInstance::SetEnvironmentCoefficient(uint32_t index, float coefficient){
	//検索キーとスロットのサイズを比較して
	if (index >= slots_.size()){
		return;
	}

	slots_[index].material.environmentCoefficient = std::clamp(coefficient, 0.0f, 1.0f);

	//変更があったので加算
	revision_++;
}

//リムライトの色の設定
void MaterialInstance::SetRimColor(const Vector4& color){
	rimLight_.color = color;

	//変更があったので加算
	revision_++;
}

//リムライトの強さの設定
void MaterialInstance::SetRimPower(float power){
	rimLight_.power = power;

	//変更があったので加算
	revision_++;
}

//リムライトのアウトラインの強さの設定
void MaterialInstance::SetRimOutLinePower(float outLinePower){
	rimLight_.outLinePower = outLinePower;

	//変更があったので加算
	revision_++;
}

//リムライトの柔らかさの設定
void MaterialInstance::SetRimSoftness(float softness){
	rimLight_.softness = softness;

	//変更があったので加算
	revision_++;
}

//リムライトをするかの設定
void MaterialInstance::SetIsRimLighting(bool enabled){
	rimLight_.enableRimLighting = static_cast<int32_t>(enabled);

	//変更があったので加算
	revision_++;
}

//リムライトの設定
void MaterialInstance::SetRimLight(const RimLight& rimLight){
	rimLight_ = rimLight;

	//変更があったので加算
	revision_++;
}

//スロットの取得
const std::vector<MaterialInstanceSlot>& MaterialInstance::GetSlots() const{
	return slots_;
}

//リムライトを取得
const RimLight& MaterialInstance::GetRimLight() const{
	return rimLight_;
}

//マテリアルが変更されたかを判断する変数の取得
uint64_t MaterialInstance::GetRevision() const{
	return revision_;
}
