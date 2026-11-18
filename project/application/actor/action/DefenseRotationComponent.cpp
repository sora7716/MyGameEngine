#include "DefenseRotationComponent.h"
#include "Logger.h"
#include "GameObject.h"
#include "MathUtility.h"
#include "Easing.h"
#include <cassert>
#include <algorithm>

//コンストラクタ
DefenseRotationComponent::DefenseRotationComponent(GameObject* gameObject) :Component(gameObject) {
}

//デストラクタ
DefenseRotationComponent::~DefenseRotationComponent() {
}

//セットアップ
void DefenseRotationComponent::Setup(GameObject* targetGameObject) {
	//対象のゲームオブジェクトを記録
	assert(targetGameObject);
	targetGameObject_ = targetGameObject;
}

//初期化
void DefenseRotationComponent::Initialize() {
}

//更新
void DefenseRotationComponent::Update() {
	//実行しない場合
	if(!isRunning_) {
		return;
	}

	//対象のゲームオブジェクトがない場合
	if(!targetGameObject_) {
		Logger::OutputLog("DefenseRotationComponentに対象となるGameObjectが設定されていません");
		return;
	}

	//回転の要求がされる
	if(rotationRequest_) {
		//回転をしていない場合のみ
		if(!isRotate_) {
			//回転のリクエストをコピー
			isRotate_ = rotationRequest_;

			//回転の開始位置(前回の位置から取得)
			beginAngle_ = degreeAngleY_;

			//Y軸を回転させる
			degreeAngleY_ = beginAngle_;
			degreeAngleY_ += kRotationStep * rotateDir_;
		}

		//リクエストをリセット
		rotationRequest_ = false;
	}

	//最終的なアングル
	float resultAngle = {};
	//回転を行う
	if(isRotate_) {
		//回転を設定
		Transform& transform = targetGameObject_->GetTransform();
		//回転の終了位置
		float targetAngle = degreeAngleY_;

		//現在の時間を取得
		currentSecond_ += mathUtility::GetDeltaTime();
		//終了位置まで線形補間
		resultAngle = std::lerp(beginAngle_,targetAngle,easing::EaseInOutExpo(currentSecond_ / rotateEndSecond_,easingSharpness_));

		//値を回転に反映
		transform.SetEulerAngleDegrees({0.0f,resultAngle,0.0f});

		//回転が終了したら
		if(currentSecond_ >= rotateEndSecond_) {
			//値のリセット
			currentSecond_ = 0.0f;
			isRotate_ = false;
			//終了位置に補正
			transform.SetEulerAngleDegrees({0.0f,targetAngle,0.0f});
		}
	}
}

//プロパティ一覧
void DefenseRotationComponent::RegisterProperties(PropertyRegistry* registry) {
	Component::RegisterProperties(registry);
	registry->Add("回転の終わる時間",rotateEndSecond_);
	registry->Add("イージングの鋭さ",easingSharpness_);
}

//複製
std::unique_ptr<Component> DefenseRotationComponent::Clone(GameObject* gameObject) const {
	std::unique_ptr<DefenseRotationComponent>cloneInstance = std::make_unique<DefenseRotationComponent>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//設定の複製
	cloneInstance->SetEnabled(this->IsEnabled());
	cloneInstance->rotateDir_ = this->rotateDir_;
	cloneInstance->degreeAngleY_ = this->degreeAngleY_;

	return cloneInstance;
}

//コンポーネントのタイプ名を返す
std::string DefenseRotationComponent::GetTypeName() const {
	return "DefenseRotationComponent";
}

//回転方向を設定
void DefenseRotationComponent::SetRotateDir(float rotateDir) {
	rotateDir_ = rotateDir;
}

//回転要求を設定
void DefenseRotationComponent::SetRotationRequest(bool request) {
	rotationRequest_ = request;
}

//実行するかの設定
void DefenseRotationComponent::SetIsRunning(bool isRunning) {
	isRunning_ = isRunning;
}
