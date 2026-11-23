#include "BowTowerScanState.h"
#include "RenderingData.h"
#include "Object3d.h"
#include "MathUtility.h"

//コンストラクタ
BowTowerScanState::BowTowerScanState() {
}

//デストラクタ
BowTowerScanState::~BowTowerScanState() {
}

//入った瞬間
void BowTowerScanState::Enter() {
}

//更新
void BowTowerScanState::Update() {
}

//終了
void BowTowerScanState::Exit() {
}

//複製
std::unique_ptr<IState> BowTowerScanState::Clone() {
	std::unique_ptr<BowTowerScanState>instance = std::make_unique<BowTowerScanState>();
	instance->Setup(gameObject_);
	return instance;
}

//攻撃範囲の角度を設定
void BowTowerScanState::SetAttackRangeDegrees(float attackRangeDegrees) {
	attackRangeDegrees_ = attackRangeDegrees;
}

//頭のアニメーション補完する際の設定
void BowTowerScanState::SetHeadTurnBlendSpeed(float headTurnBlendSpeed) {
	headTurnBlendSpeed_ = headTurnBlendSpeed;
}

//探しているときのアニメーションの初期化
void BowTowerScanState::InitializeScanAnimation() {
	scanRotOffset_ = 0.0f;
	scanDir_ = 1.0f;
}

//探してる時のアニメーション
void BowTowerScanState::UpdateScanAnimation() {
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
	float halfAttackRange = attackRangeDegrees_ / 2.0f;
	//向きの切り替え
	if(scanRotOffset_ >= maxScanRotate_ - halfAttackRange) {
		scanDir_ = -1.0f;
	} else if(scanRotOffset_ <= minScanRotate_ + halfAttackRange) {
		scanDir_ = 1.0f;
	}
}