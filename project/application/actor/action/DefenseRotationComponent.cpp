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

//初期化
void DefenseRotationComponent::Initialize() {
}

//更新
void DefenseRotationComponent::Update() {
	//実行しない場合
	if(!isRunning_) {
		return;
	}

	//回転の要求がされる
	if(rotationRequest_) {
		//回転をしていない場合のみ
		if(!isRotate_) {
			//回転のリクエストをコピー
			isRotate_ = rotationRequest_;

			//回転の開始位置(前回の位置から取得)
			beginAngleY_ = degreeAngleY_;

			//Y軸を回転させる
			degreeAngleY_ = beginAngleY_;
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
		Transform& transform = GetOwner()->GetTransform();
		//回転の終了位置
		float targetAngle = degreeAngleY_;

		//現在の時間を取得
		currentSecond_ += mathUtility::GetDeltaTime();
		//終了位置まで線形補間
		resultAngle = std::lerp(beginAngleY_,targetAngle,easing::EaseInOutExpo(currentSecond_ / rotateEndSecond_,easingSharpness_));

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
	cloneInstance->rotationRequest_ = this->rotationRequest_;
	cloneInstance->isRotate_ = this->isRotate_;
	cloneInstance->currentSecond_ = this->currentSecond_;
	cloneInstance->rotateEndSecond_ = this->rotateEndSecond_;
	cloneInstance->isRunning_ = this->isRunning_;
	cloneInstance->beginAngleY_ = this->beginAngleY_;
	cloneInstance->easingSharpness_ = this->easingSharpness_;

	return cloneInstance;
}

//回転のリセット
void DefenseRotationComponent::ResetRotation() {
	//角度を戻す
	degreeAngleY_ = 0.0f;
	beginAngleY_ = 0.0f;
	//回転の途中経過を消す
	isRotate_ = false;
	rotationRequest_ = false;
	currentSecond_ = 0.0f;
	//見た目の角度を戻す
	GetOwner()->GetTransform().SetEulerAngleDegrees(Vector3::GetZero());
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
