#include "BowTowerScanState.h"
#include "GameObject.h"
#include "Object3d.h"
#include "BowTower.h"
#include "RenderingData.h"
#include "MathUtility.h"

//コンストラクタ
BowTowerScanState::BowTowerScanState() {
}

//デストラクタ
BowTowerScanState::~BowTowerScanState() {
}

//セットアップ
void BowTowerScanState::Setup(GameObject* gameObject) {
	IState::Setup(gameObject);
	//弓のタワーを取得
	bowTower_ = gameObject_->GetComponent<BowTower>();
}

//入った瞬間
void BowTowerScanState::Enter() {
	InitializeScanAnimation();
}

//更新
void BowTowerScanState::Update() {
	UpdateScanAnimation();
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
	const float blend = std::clamp(bowTower_->GetHeadTurnBlendSpeed() * mathUtility::GetDeltaTime(),0.0f,1.0f);

	//補間
	nodeTransform.rotate = Quaternion::Slerp(currentRotation,nodeTransform.rotate,blend);

	//取得したTransformを送信
	object3d_->SetNodeLocalTransform("BowTower/head",nodeTransform);

	//攻撃範囲の半分
	float halfAttackRange = bowTower_->GetAttackRangeDegrees() / 2.0f;
	//向きの切り替え
	if(scanRotOffset_ >= maxScanRotate_ - halfAttackRange) {
		scanDir_ = -1.0f;
	} else if(scanRotOffset_ <= minScanRotate_ + halfAttackRange) {
		scanDir_ = 1.0f;
	}
}