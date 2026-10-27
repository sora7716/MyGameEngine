#include "SpotLight.h"
#include "GameObject.h"
#include <algorithm>

//コンストラクタ
SpotLight::SpotLight(GameObject* gameObject) :Component(gameObject){
}

//デストラクタ
SpotLight::~SpotLight(){
}

//初期化
void SpotLight::Initialize(){
	Component::Initialize();
}

//更新
void SpotLight::Update(){
	Component::Update();
	//小さい値
	constexpr float kEpsilon = 0.001f;
	//光の強さが0を下回らないように設定
	intensity_ = std::max(intensity_, 0.0f);
	//ライトの届く最大距離が0にならないように
	distance_ = std::max(distance_, kEpsilon);
	//減衰率が0にならないように
	decay_ = std::max(decay_, kEpsilon);
	//cosを制限
	cosAngle_ = std::clamp(cosAngle_, -1.0f, 1 - kEpsilon);
	cosFalloffStart_ = std::clamp(cosFalloffStart_, cosAngle_ + kEpsilon, 1.0f);
	//光の向きを正規化
	Vector3 lightDir = { 0.0f,0.0f,-1.0f };
	direction_ = GetOwner()->GetTransform().rotate.RotateVector(lightDir);
}

//複製
std::unique_ptr<Component> SpotLight::Clone(GameObject* gameObject) const{
	std::unique_ptr<SpotLight>cloneInstance = std::make_unique<SpotLight>(gameObject);

	//初期化
	cloneInstance->Initialize();
	cloneInstance->color_ = this->color_;
	cloneInstance->enableLighting_ = this->enableLighting_;
	cloneInstance->distance_ = this->distance_;
	cloneInstance->decay_ = this->decay_;
	cloneInstance->intensity_ = this->intensity_;
	cloneInstance->cosAngle_ = cosAngle_;
	cloneInstance->cosFalloffStart_ = cosFalloffStart_;

	//持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	return cloneInstance;
}

//プロパティ一覧を登録
void SpotLight::RegisterProperties(PropertyRegistry* registry){
	Component::RegisterProperties(registry);
	registry->Add("enableLight", enableLighting_);
	registry->Add("color", color_);
	registry->Add("intensity", intensity_);
	registry->Add("distance", distance_);
	registry->Add("decay", decay_);
	registry->Add("cosAngle", cosAngle_, 0.01f);
	registry->Add("cosFalloffStart", cosFalloffStart_, 0.01f);
}

//コンポーネントのタイプ名を返す
std::string SpotLight::GetTypeName() const{
	return "スポットライト";
}

//スポットライトのデータを取得
lightData::SpotLight SpotLight::GetSpotLightData() const{
	lightData::SpotLight spotLight = {};
	spotLight.color = color_;
	spotLight.cosAngle = cosAngle_;
	spotLight.cosFalloffStart = cosFalloffStart_;
	spotLight.position = GetOwner()->GetWorldPos();
	spotLight.decay = decay_;
	spotLight.direction = direction_;
	spotLight.distance = distance_;
	spotLight.intensity = intensity_;
	spotLight.enable = enableLighting_;
	return spotLight;
}
