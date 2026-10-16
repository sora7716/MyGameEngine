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
	directionalLight_.intensity = std::max(directionalLight_.intensity, 0.0f);
	////光の向きを正規化
	//Vector3 lightDir = { 0.0f,0.0f,1.0f };
	//mathUtility::TransformNormal(lightDir,);
	//directionalLight_.direction = directionalLight_.direction.Normalize();
}

//複製
std::unique_ptr<Component> DirectionalLight::Clone(GameObject* gameObject) const{
	std::unique_ptr<DirectionalLight>cloneInstance = std::make_unique<DirectionalLight>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//Playerが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	return cloneInstance;
}

//コンポーネントのタイプ名を返す
std::string DirectionalLight::GetTypeName() const{
	return "平行光源";
}
