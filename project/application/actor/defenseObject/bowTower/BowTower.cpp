#include "BowTower.h"
#include "MathUtility.h"
#include "GameObject.h"
#include "Object3d.h"
#include "Logger.h"
#include "Collision.h"
#include <cmath>

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
	//探索範囲を設定
	scanRange_.origin = gameObject_->GetWorldPos();
	scanRange_.radius = scanRadius_;
	object3d_->TryGetNodeModelMatrix("BowTower/head",headModelMatrix_);
	scanRange_.forward = mathUtility::TransformNormal(Vector3::GetForward(),headModelMatrix_);
	scanRange_.sweepAngleDegrees = maxScanRotate_ * 2.0f;
	//探してる時のアニメーション
	UpdateScanAnimation();

	//対象が設定されていなければ
	if(!target_) {
		Logger::OutputLog("BowTowerに攻撃対象が設定されていません");
		return;
	}

	//探索範囲に入っていた場合
	if(collision::IsCollision(scanRange_,target_->GetWorldPos())) {
		flag_ = true;
	} else {
		flag_ = false;
	}
}

//プロパティ一覧
void BowTower::RegisterProperties(PropertyRegistry* registry) {
	Component::RegisterProperties(registry);
	registry->BeginGroup("探索アニメーション");
	registry->Add("探索範囲の最大角度",maxScanRotate_);
	registry->Add("探索範囲の最小角度",minScanRotate_);
	registry->Add("探索する速度",scanSpeed_);
	registry->EndGroup();
	registry->Add("探索する半径",scanRadius_);
	registry->Add("確認用フラグ",flag_);
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

//スキャンする半径の取得
float BowTower::GetScanRadius() const {
	return scanRadius_;
}

//探しているときのアニメーションの初期化
void BowTower::InitializeScanAnimation() {
	//探索開始時に角度を取得
	Transform nodeTransform = object3d_->GetNodeLocalTransform("BowTower/head");
	scanBeginRotate = nodeTransform.GetDegreeAngle();

}

//探してる時のアニメーション
void BowTower::UpdateScanAnimation() {
	//オフセットを補間
	scanRotOffset_ += scanSpeed_ * scanDir_;

	//頭のTransformを取得
	Transform nodeTransform = object3d_->GetNodeLocalTransform("BowTower/head");
	nodeTransform.SetEulerAngleDegrees({scanBeginRotate.x,scanBeginRotate.y + scanRotOffset_,scanBeginRotate.z});

	//取得したTransformを送信
	object3d_->SetNodeLocalTransform("BowTower/head",nodeTransform);

	//向きの切り替え
	if(scanRotOffset_ >= maxScanRotate_) {
		scanDir_ = -1.0f;
	} else if(scanRotOffset_ <= minScanRotate_) {
		scanDir_ = 1.0f;
	}
}
