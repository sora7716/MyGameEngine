#include "BowTower.h"
#include "MathUtility.h"
#include "GameObject.h"
#include "Object3d.h"
#include "Logger.h"
#include "Collision.h"
#include "Quaternion.h"
#include <cmath>
#include <algorithm>

//コンストラクタ
BowTower::BowTower(GameObject* gameObject) :BaseDefenseObject(gameObject) {
}

//デストラクタ
BowTower::~BowTower() {
}

//初期化
void BowTower::Initialize() {
	//ゲームオブジェクトを取得
	gameObject_ = GetOwner();
	//object3d_を取得
	object3d_ = gameObject_->GetComponent<Object3d>();
	if(!object3d_) {
		Logger::OutputLog("object3d_が取得できませんでした");
		assert(false);
	}
}

//更新
void BowTower::Update() {
	//対象が設定されていなければ
	if(!target_) {
		Logger::OutputLog("BowTowerに攻撃対象が設定されていません");
		return;
	}

	//探索範囲を設定
	SettingScanRange();
	//探索範囲に入っているか
	isInScanRange_ = collision::IsCollision(scanRange_,target_->GetWorldPos());

	//攻撃範囲に入っているか
	if(isInScanRange_) {
		//入っていた場合
		//攻撃範囲を設定
		SettingAttackRange();
		//攻撃フェーズじゃなければ
		if(behavior_ != Behavior::kAttack) {
			//衝突判定
			if(collision::IsCollision(attackRange_,target_->GetWorldPos())) {
				behaviorRequest_ = Behavior::kAttack;
			}
		}
	} else {
		//現在の振る舞いが探索じゃなければ
		if(behavior_ != Behavior::kScan) {
			//入っていなかった場合
			behaviorRequest_ = Behavior::kScan;
		}
	}

	//要求がNoneじゃなければ
	if(behaviorRequest_ != Behavior::kNone) {
		//要求をコピー
		behavior_ = behaviorRequest_;
		//要求をリセット
		behaviorRequest_ = Behavior::kNone;

		//初期化
		switch(behavior_) {
			case Behavior::kScan:
				//探しているときの初期化
				InitializeScanAnimation();
				break;
			case Behavior::kAttack:
				//攻撃しているときの初期化
				InitializeAttackAnimation();
				break;
		}
	}

	//動きの更新
	switch(behavior_) {
		case Behavior::kScan:
			//探してる時のアニメーション
			UpdateScanAnimation();
			break;
		case Behavior::kAttack:
			//攻撃しているときのアニメーション
			UpdateAttackAnimation();
			break;
	}
}

//プロパティ一覧
void BowTower::RegisterProperties(PropertyRegistry* registry) {
	Component::RegisterProperties(registry);
	registry->BeginGroup("探索アニメーション");
	registry->Add("探索範囲の最大角度",maxScanRotate_);
	registry->Add("探索範囲の最小角度",minScanRotate_);
	registry->Add("探索する速度",scanSpeed_);
	registry->Add("探索する半径",scanRadius_);
	registry->EndGroup();

	registry->BeginGroup("攻撃アニメーション");
	registry->Add("攻撃する感覚",attackInterval_);
	registry->Add("反動の最大角度",recoilMaxDegrees);
	registry->EndGroup();
}

//複製
std::unique_ptr<Component> BowTower::Clone(GameObject* gameObject) const {
	std::unique_ptr<BowTower>cloneInstance = std::make_unique<BowTower>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//設定を複製
	cloneInstance->SetEnabled(this->IsEnabled());

	return cloneInstance;
}

//コンポーネントのタイプ名を返す
std::string BowTower::GetTypeName() const {
	return "BowTower";
}

//攻撃対象を設定
void BowTower::SetTarget(GameObject* target) {
	target_ = target;
}

//矢を発射の要求を設定
void BowTower::SetFireRequest(bool request) {
	fireRequest_ = request;
}

//スキャンする半径の取得
float BowTower::GetScanRadius() const {
	return scanRadius_;
}

//弓を発射するかを取得
bool BowTower::IsFire() const {
	return fireRequest_;
}

//攻撃範囲を設定
void BowTower::SettingAttackRange() {
	//範囲を設定
	attackRange_.origin = gameObject_->GetWorldPos();
	//どれくらいの範囲かを設定
	attackRange_.radius = scanRadius_;
	//頭のモデル行列を取得
	object3d_->TryGetNodeModelMatrix("BowTower/head",headModelMatrix_);
	//頭のワールド行列を取得
	Matrix4x4 headWorldMatrix = headModelMatrix_ * gameObject_->GetWorldMatrix();
	//頭のワールド行列から前方を取得
	attackRange_.forward = mathUtility::TransformNormal(Vector3::GetBack(),headWorldMatrix);
	//範囲の角度を設定
	attackRange_.sweepAngleDegrees = attackRangeAngle_;
}

//探索範囲を設定
void BowTower::SettingScanRange() {
	//範囲を設定
	scanRange_.origin = gameObject_->GetWorldPos();
	//どれくらいの範囲かを設定
	scanRange_.radius = scanRadius_;
	//ワールド行列から前方を取得
	scanRange_.forward = mathUtility::TransformNormal(Vector3::GetBack(),gameObject_->GetWorldMatrix());
	//範囲の角度を設定
	scanRange_.sweepAngleDegrees = maxScanRotate_ * 2.0f;
}

//探しているときのアニメーションの初期化
void BowTower::InitializeScanAnimation() {
	scanRotOffset_ = 0.0f;
	scanDir_ = 1.0f;
}

//探してる時のアニメーション
void BowTower::UpdateScanAnimation() {
	//頭のTransformを取得
	Transform nodeTransform = object3d_->GetNodeLocalTransform("BowTower/head");

	//現在の回転を保存
	const Quaternion currentRotation = nodeTransform.rotate;

	//オフセットを補間
	scanRotOffset_ += scanSpeed_ * scanDir_;
	//目標姿勢を作成
	nodeTransform.SetEulerAngleDegrees({scanBeginRotate.x,scanBeginRotate.y + scanRotOffset_,scanBeginRotate.z});

	//ブレンド(補間数係数)を作成
	const float blend = std::clamp(headTurnBlendSpeed_ * mathUtility::GetDeltaTime(),0.0f,1.0f);

	//補間
	nodeTransform.rotate = Quaternion::Slerp(currentRotation,nodeTransform.rotate,blend);

	//取得したTransformを送信
	object3d_->SetNodeLocalTransform("BowTower/head",nodeTransform);

	//攻撃範囲の半分
	float halfAttackRange = attackRangeAngle_ / 2.0f;
	//向きの切り替え
	if(scanRotOffset_ >= maxScanRotate_ - halfAttackRange) {
		scanDir_ = -1.0f;
	} else if(scanRotOffset_ <= minScanRotate_ + halfAttackRange) {
		scanDir_ = 1.0f;
	}
}

//攻撃するときのアニメーションの初期化
void BowTower::InitializeAttackAnimation() {
	//タイマーをリセット
	attackTimer_ = 0.0f;
	recoilTimer_ = 0.0f;
}

//攻撃するときのアニメーションの更新
void BowTower::UpdateAttackAnimation() {
	//頭のTransformを取得
	Transform nodeTransform = object3d_->GetNodeLocalTransform("BowTower/head");
	//現在の回転を保存
	const Quaternion currentRotation = nodeTransform.rotate;

	//デルタタイムを設定
	const float deltaTime = mathUtility::GetDeltaTime();

	//攻撃時間を管理するタイマーを計測
	attackTimer_ += deltaTime;
	//反動を管理するタイマーを計測
	recoilTimer_ += deltaTime;

	//インターバルが終了したら
	if(attackTimer_ >= attackInterval_) {
		//タイマーをリセット
		attackTimer_ = 0.0f;
		recoilTimer_ = 0.0f;
		//矢の発射の要求
		fireRequest_ = true;
	}

	//攻撃開始になったら
	float recoilDegrees = 0.0f;//攻撃時の角度
	//反動の最大時間
	float recoilMaxSecond = 0.05f;
	//立て直しの最大時間
	float recoverMaxSecond = 0.2f;
	if(recoilTimer_ < recoilMaxSecond) {
		//反動
		const float progress = recoilTimer_ / recoilMaxSecond;
		recoilDegrees = std::lerp(0.0f,recoilMaxDegrees,progress);
	} else if(recoilTimer_ < recoverMaxSecond) {
		//立て直し
		const float progress = (recoilTimer_ - recoilMaxSecond) / recoverMaxSecond - recoilMaxSecond;
		recoilDegrees = std::lerp(recoilMaxDegrees,0.0f,progress);
	}
	//頭の角度
	const Vector3 headDegrees = nodeTransform.GetDegreeAngle();
	nodeTransform.SetEulerAngleDegrees({recoilDegrees,headDegrees.y,headDegrees.z});

	//敵への方向を取得
	Vector3 toEnemy = target_->GetWorldPos() - attackRange_.origin;
	//横のずれ
	float sizeOffset = attackRange_.forward.Cross(toEnemy).y;
	//正面のずれ
	float forwardDot = attackRange_.forward.Dot(toEnemy);
	//符号付のずれ角
	float signedOffsetAngle = std::atan2(sizeOffset,forwardDot);

	//次の向く角度を設定
	float nextHeadYAngle = nodeTransform.GetEulerAngle().y + signedOffsetAngle;
	nodeTransform.SetEulerAngle({nodeTransform.GetEulerAngle().x,nextHeadYAngle,nodeTransform.GetEulerAngle().z});

	//ブレンド(補間数係数)を作成
	const float blend = std::clamp(headTurnBlendSpeed_ * mathUtility::GetDeltaTime(),0.0f,1.0f);

	//補間
	nodeTransform.rotate = Quaternion::Slerp(currentRotation,nodeTransform.rotate,blend);

	//取得したTransformを送信
	object3d_->SetNodeLocalTransform("BowTower/head",nodeTransform);
}
