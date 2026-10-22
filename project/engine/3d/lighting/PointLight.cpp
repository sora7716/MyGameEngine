#include "PointLight.h"
#include "GameObject.h"

//コンストラクタ
PointLight::PointLight(GameObject* gameObject) :Component(gameObject){
}

//デストラクタ
PointLight::~PointLight(){
}

//初期化
void PointLight::Initialize(){
	Component::Initialize();
}

//更新
void PointLight::Update(){
	Component::Update();
	//小さい値
	constexpr float kEpsilon = 0.001f;
	//光の強さが0を下回らないように設定
	intensity_ = std::max(intensity_, 0.0f);
	//ライトの届く最大距離が0にならないように
	distance_ = std::max(distance_, kEpsilon);
	//減衰率が0にならないように
	decay_ = std::max(decay_, kEpsilon);
}

//複製
std::unique_ptr<Component> PointLight::Clone(GameObject* gameObject) const{
	std::unique_ptr<PointLight>cloneInstance = std::make_unique<PointLight>(gameObject);

	//初期化
	cloneInstance->Initialize();
	cloneInstance->color_ = this->color_;
	cloneInstance->enableLighting_ = this->enableLighting_;
	cloneInstance->distance_ = this->distance_;
	cloneInstance->decay_ = this->decay_;
	cloneInstance->isLambert_ = this->isLambert_;
	cloneInstance->isBlinnPhong_ = this->isBlinnPhong_;
	cloneInstance->intensity_ = this->intensity_;

	//持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	return cloneInstance;
}

//プロパティ一覧
void PointLight::RegisterProperties(PropertyRegistry* registry){
	Component::RegisterProperties(registry);
	registry->Add("enableLighting", enableLighting_);
	registry->Add("color", color_);
	registry->Add("intensity", intensity_);
	registry->Add("isLambert", isLambert_);
	registry->Add("isBlinnPhong", isBlinnPhong_);
	registry->Add("distance", distance_);
	registry->Add("decay", decay_);
}

//コンポーネントのタイプ名を返す
std::string PointLight::GetTypeName() const{
	return "点光源";
}

//点光源のデータを取得
lightData::PointLight PointLight::GetPointLightData() const{
	lightData::PointLight pointLightData = {};
	pointLightData.color = color_;
	pointLightData.position = GetOwner()->GetWorldPos();
	pointLightData.distance = distance_;
	pointLightData.decay = decay_;
	pointLightData.enable = enableLighting_;
	pointLightData.isLambert = isLambert_;
	pointLightData.intensity = intensity_;
	pointLightData.isBlinnPhong = isBlinnPhong_;
	return pointLightData;
}
