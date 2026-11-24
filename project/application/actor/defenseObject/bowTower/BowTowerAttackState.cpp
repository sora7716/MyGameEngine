#include "BowTowerAttackState.h"
#include "GameObject.h"
#include "Object3d.h"
#include "BowTower.h"
#include "MathUtility.h"
#include <algorithm>

//コンストラクタ
BowTowerAttackState::BowTowerAttackState() {
}

//デストラクタ
BowTowerAttackState::~BowTowerAttackState() {
}

//セットアップ
void BowTowerAttackState::Setup(GameObject* gameObject) {
	IState::Setup(gameObject);
	//弓のタワーを取得
	bowTower_ = gameObject_->GetComponent<BowTower>();
}

//入った瞬間
void BowTowerAttackState::Enter() {
	InitializeAttackAnimation();
}

//更新
void BowTowerAttackState::Update() {
	UpdateAttackAnimation();
}

//終了
void BowTowerAttackState::Exit() {
}

//複製
std::unique_ptr<IState> BowTowerAttackState::Clone() {
	std::unique_ptr<BowTowerAttackState>instance = std::make_unique<BowTowerAttackState>();
	instance->Setup(gameObject_);
	return instance;
}

//攻撃するときのアニメーションの初期化
void BowTowerAttackState::InitializeAttackAnimation() {
	//タイマーをリセット
	attackTimer_ = attackInterval_;
	recoilTimer_ = kRecoilMaxSecond;
}

//攻撃するときのアニメーションの更新
void BowTowerAttackState::UpdateAttackAnimation() {
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
		bowTower_->SetFireRequest(true);
	}

	//攻撃開始になったら
	float recoilDegrees = 0.0f;//攻撃時の角度
	if(recoilTimer_ < kRecoilMaxSecond) {
		//反動
		const float progress = recoilTimer_ / kRecoilMaxSecond;
		recoilDegrees = std::lerp(0.0f,recoilMaxDegrees,progress);
	} else if(recoilTimer_ < kRecoverMaxSecond) {
		//立て直し
		const float progress = (recoilTimer_ - kRecoilMaxSecond) / kRecoverMaxSecond - kRecoilMaxSecond;
		recoilDegrees = std::lerp(recoilMaxDegrees,0.0f,progress);
	}
	//頭の角度
	const Vector3 headDegrees = nodeTransform.GetDegreeAngle();
	nodeTransform.SetEulerAngleDegrees({recoilDegrees,headDegrees.y,headDegrees.z});

	//敵への方向を取得
	Vector3 toEnemy = bowTower_->GetAttackTarget()->GetWorldPos() - bowTower_->GetAttackRange().origin;
	//横のずれ
	float sizeOffset = bowTower_->GetAttackRange().forward.Cross(toEnemy).y;
	//正面のずれ
	float forwardDot = bowTower_->GetAttackRange().forward.Dot(toEnemy);
	//符号付のずれ角
	float signedOffsetAngle = std::atan2(sizeOffset,forwardDot);

	//次の向く角度を設定
	float nextHeadYAngle = nodeTransform.GetEulerAngle().y + signedOffsetAngle;
	nodeTransform.SetEulerAngle({nodeTransform.GetEulerAngle().x,nextHeadYAngle,nodeTransform.GetEulerAngle().z});

	//ブレンド(補間数係数)を作成
	const float blend = std::clamp(bowTower_->GetHeadTurnBlendSpeed() * mathUtility::GetDeltaTime(),0.0f,1.0f);

	//補間
	nodeTransform.rotate = Quaternion::Slerp(currentRotation,nodeTransform.rotate,blend);

	//取得したTransformを送信
	object3d_->SetNodeLocalTransform("BowTower/head",nodeTransform);
}