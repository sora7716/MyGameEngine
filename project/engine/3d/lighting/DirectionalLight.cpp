#define NOMINMAX
#include "DirectionalLight.h"
#include "MathUtility.h"
#include "Vector3.h"
#include "GameObject.h"
#include "MathUtility.h"
#include <algorithm>

//コンストラクタ
DirectionalLight::DirectionalLight(GameObject* gameObject) :Component(gameObject){
}

//デストラクタ
DirectionalLight::~DirectionalLight(){
}

//初期化
void DirectionalLight::Initialize(){
}

//更新
void DirectionalLight::Update(){
	//光の強さが0を下回らないように設定
	intensity_ = std::max(intensity_, 0.0f);
	//光の向きを正規化
	Vector3 lightDir = { 0.0f,0.0f,-1.0f };
	direction_ = GetOwner()->GetTransform().rotate.RotateVector(lightDir);
}

//複製
std::unique_ptr<Component> DirectionalLight::Clone(GameObject* gameObject) const{
	std::unique_ptr<DirectionalLight>cloneInstance = std::make_unique<DirectionalLight>(gameObject);

	//初期化
	cloneInstance->Initialize();
	cloneInstance->color_ = this->color_;
	cloneInstance->direction_ = this->direction_;
	cloneInstance->enableLighting_ = this->enableLighting_;
	cloneInstance->isLambert_ = this->isLambert_;
	cloneInstance->isBlinnPhong_ = this->isBlinnPhong_;
	cloneInstance->intensity_ = this->intensity_;

	//Playerが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	return cloneInstance;
}

//プロパティ一覧の登録
void DirectionalLight::RegisterProperties(PropertyRegistry* registry){
	Component::RegisterProperties(registry);
	registry->Add("enableLighting", enableLighting_);
	registry->Add("color", color_);
	registry->Add("intencity", intensity_);
	registry->Add("isLambert", isLambert_);
	registry->Add("isBlinnPhong", isBlinnPhong_);
}

//コンポーネントのタイプ名を返す
std::string DirectionalLight::GetTypeName() const{
	return "平行光源";
}

//ライトの有効/無効の設定
void DirectionalLight::SetEnabledLighting(bool enabledLighting){
	enableLighting_ = enabledLighting;
}

//ランバート反射モデルにするか
void DirectionalLight::SetIsLambert(bool isLambert){
	isLambert_ = isLambert;
}

//Blinn-Phong反射に切り替えるか
void DirectionalLight::SetIsBlinnPhong(bool isBlinnPhong){
	isBlinnPhong_ = isBlinnPhong;
}

//光の強さを設定
void DirectionalLight::SetIntensity(float intensity){
	intensity_ = intensity;
}

//色を設定
void DirectionalLight::SetColor(const Color& color){
	color_ = color;
}

//平行光源のデータを取得
lightData::DirectionalLight DirectionalLight::GetDirectionalLightData()const{
	lightData::DirectionalLight directionalLightData = {};
	directionalLightData.color = color_;
	directionalLightData.direction = direction_;
	directionalLightData.enable = enableLighting_;
	directionalLightData.intensity = intensity_;
	directionalLightData.isBlinnPhong = isBlinnPhong_;
	directionalLightData.isLambert = isLambert_;
	return directionalLightData;
}
