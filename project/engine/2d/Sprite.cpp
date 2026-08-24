#include "Sprite.h"
#include "SpriteCommon.h"
#include <cassert>
#include "MatrixUtility.h"
#include "TextureManager.h"
#include "DirectXBase.h"
#include "WinApi.h"
#include "WorldTransform.h"
#include "ImGuiManager.h"

//コンストラクタ
Sprite::Sprite(GameObject* gameObject) :Component(gameObject){
}

//デストラクタ
Sprite::~Sprite(){
}

//初期化
void Sprite::Initialize(){
	//基底クラスの初期化
	Component::Initialize();
	//スプライトファイルパスを記録
	imageFileName_ = "engine/resources/textures/white1x1.png";
}

//更新
void Sprite::Update(){
	//ワールド座標の更新
	UpdateTransform();

	//UV座標の更新
	UpdateUVTransform();

	//描画データのセットアップ
	SetupRenderData();
}

//複製
std::unique_ptr<Component> Sprite::Clone(GameObject* gameObject) const{
	std::unique_ptr<Sprite>cloneInstance = std::make_unique<Sprite>(gameObject
	);

	//初期化
	cloneInstance->Initialize();

	//SkyBoxが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	cloneInstance->imageFileName_ = this->imageFileName_;
	cloneInstance->blendMode_ = this->blendMode_;
	cloneInstance->material_ = this->material_;
	cloneInstance->uvTransform_ = uvTransform_;
	cloneInstance->transformationMatrix_ = transformationMatrix_;
	return cloneInstance;
}

//テクスチャの変更
void Sprite::ChangeTexture(const std::string& spriteName){
	imageFileName_ = "engine/resources/textures/" + spriteName;
}

//色のセッター
void Sprite::SetColor(const Vector4& color){
	material_.color = color;
}

//ブレンドモードのセッター
void Sprite::SetBlendMode(BlendMode blendMode){
	blendMode_ = blendMode;
}

//UVスケールの設定
void Sprite::SetUVScale(const Vector2& scale){
	uvTransform_.scale = scale;
}

//UV回転の設定
void Sprite::SetUVRotate(float rotate){
	uvTransform_.rotate = rotate;
}

//UV平行移動の設定
void Sprite::SetUVTranslate(const Vector2& translate){
	uvTransform_.translate = translate;
}

//UVのトランスフォームの設定
void Sprite::SetUVRectTransform(const RectTransform& rectTransform){
	uvTransform_ = rectTransform;
}

//UVスケールの取得
const Vector2& Sprite::GetUVScale(){
	return uvTransform_.scale;
}

//UV回転の取得
float Sprite::GetUVRotate(){
	return uvTransform_.rotate;
}

//UV平行移動の取得
const Vector2& Sprite::GetUVTranslate(){
	return uvTransform_.translate;
}

//UVのトランスフォームの取得
const RectTransform& Sprite::GetUVRectTransform(){
	return uvTransform_;
}

//描画データの取得
const SpriteRenderData& Sprite::GetRenderData(){
	return renderData_;
}

//ワールド座標の更新
void Sprite::UpdateTransform(){
	GameObject* gameObject = GetOwner();
	transformationMatrix_.world = matrixUtility::MakeAffineMatrix(gameObject->GetTransform());
	//ProjectionMatrixを作って平行投影行列を書き込む
	const Matrix4x4& projectionMatrix = matrixUtility::MakeOrthographicMatrix(0.0f, 0.0f, static_cast<float>(WinApi::kClientWidth), static_cast<float>(WinApi::kClientHeight), 0.1f, 100.0f);
	//wvpの書き込み
	const Matrix4x4& viewProjectionMatrix = Matrix4x4::Identity4x4() * projectionMatrix;
	transformationMatrix_.wvp = transformationMatrix_.world * viewProjectionMatrix;
}

// UVの座標変換の更新
void Sprite::UpdateUVTransform(){
	//UVTransform
	material_.uvMatrix = matrixUtility::MakeAffineMatrix(uvTransform_);
}

//描画に必要なデータのセットアップ
void Sprite::SetupRenderData(){
	GameObject* gameObject = GetOwner();
	renderData_.blendMode = blendMode_;
	renderData_.isActive = gameObject->IsActive();
	renderData_.material = material_;
	renderData_.transformationMatrix = transformationMatrix_;
}