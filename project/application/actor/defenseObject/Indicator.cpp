#include "Indicator.h"
#include "BaseCollider.h"
#include "GameObject.h"
#include "MathUtility.h"
#include "Object3d.h"
#include "Logger.h"
#include "Easing.h"
#include <cassert>

//コンストラクタ
Indicator::Indicator(GameObject* gameObject) :Component(gameObject) {
}

//デストラクタ
Indicator::~Indicator() {
}

//初期化
void Indicator::Initialize() {
	//ゲームオブジェクトを記録
	gameObject_ = GetOwner();
}

//更新
void Indicator::Update() {
	//振る舞いの要求に何か入っていたら
	if(behaviorRequest_ != Behavior::kNone) {
		//リクエストをコピー
		behavior_ = behaviorRequest_;
		//リクエストリセット
		behaviorRequest_ = Behavior::kNone;
	}

	//振る舞いを変更する
	switch(behavior_) {
		case Indicator::kVisibility:
			VisibilityAnimation();
			break;
		case Indicator::kHidden:
			HiddenAnimation();
			break;
	}
}

//プロパティ一覧を登録
void Indicator::RegisterProperties(PropertyRegistry* registry) {
	Component::RegisterProperties(registry);
	registry->Add("終わる時間(秒)",endSecond_);
}

//接触した瞬間
void Indicator::OnTriggerEnter(BaseCollider* other) {
	//プレイヤーが接触したら
	if(other->GetOwner()->GetTag() == "Player") {
		//表示の要求
		behaviorRequest_ = Behavior::kVisibility;
		//現在の時間をリセット
		currentSecond_ = 0.0f;
		//衝突した瞬間のScaleを取得
		scaleAtCollision_ = visual_->GetTransform().scale;
	}
}

//離れた瞬間
void Indicator::OnTriggerExit(BaseCollider* other) {
	//プレイヤーが離れたら
	if(other->GetOwner()->GetTag() == "Player") {
		//非表示を要求の要求
		behaviorRequest_ = Behavior::kHidden;
		//現在の時間をリセット
		currentSecond_ = 0.0f;
		//衝突した範囲から抜ける時のScaleを取得
		scaleAtCollisionExit_ = visual_->GetTransform().scale;
	}
}

//複製
std::unique_ptr<Component> Indicator::Clone(GameObject* gameObject) const {
	std::unique_ptr<Indicator>cloneInstance = std::make_unique<Indicator>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//設定の複製
	cloneInstance->SetEnabled(this->IsEnabled());

	return cloneInstance;
}

//コンポーネントのタイプ名を返す
std::string Indicator::GetTypeName() const {
	return "Indicator";
}

//最終的は目標の半径
void Indicator::SetShowTargetRadius(float radius) {
	showTargetRadius_ = radius;
}

//見た目だけのゲームオブジェクトを設定
void Indicator::SetVisualObject(GameObject* visual) {
	assert(visual);
	visual_ = visual;
	//オブジェクト3dの取得
	object3d_ = visual_->GetComponent<Object3d>();
	if(!object3d_) {
		Logger::OutputLog("IndicatorにObject3dが追加されていません");
		assert(false);
	}
}

//表示用のアニメーション
void Indicator::VisibilityAnimation() {
	//表示をする
	object3d_->SetEnabled(true);
	Transform& transform = visual_->GetTransform();
	//目標
	Vector3 targetScale = {showTargetRadius_,showTargetRadius_,showTargetRadius_};
	if(currentSecond_ < endSecond_) {
		//現在の時間が終わる時間より小さければ
		//現在の時間を計測
		currentSecond_ += mathUtility::GetDeltaTime();
		//線形補間
		transform.scale = Vector3::Lerp(scaleAtCollision_,targetScale,easing::EaseInOutQuart(currentSecond_ / endSecond_));
	} else {
		//現在の時間が終わる時間以上なら
		//時間を固定
		currentSecond_ = endSecond_;
		//スケールも固定
		transform.scale = targetScale;
	}
}

//非表示用のアニメーション
void Indicator::HiddenAnimation() {
	Transform& transform = visual_->GetTransform();
	if(currentSecond_ < endSecond_) {
		//現在の時間が終わる時間より小さければ
		//現在の時間を計測
		currentSecond_ += mathUtility::GetDeltaTime();
		//線形補間
		transform.scale = Vector3::Lerp(scaleAtCollisionExit_,scaleAtCollision_,easing::EaseInOutQuart(currentSecond_ / endSecond_));
	} else {
		//現在の時間が終わる時間以上なら
		//時間を固定
		currentSecond_ = endSecond_;
		//スケールも固定
		transform.scale = scaleAtCollision_;
		//非表示にする
		object3d_->SetEnabled(false);
	}
}
