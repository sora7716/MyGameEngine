#include "BowTower.h"
#include "MathUtility.h"
#include "GameObject.h"
#include "Object3d.h"
#include "Logger.h"
#include <cmath>

//コンストラクタ
BowTower::BowTower(GameObject* gameObject) :BaseDefenseObject(gameObject) {
}

//デストラクタ
BowTower::~BowTower() {
}

//初期化
void BowTower::Initialize() {
}

//更新
void BowTower::Update() {
	UpdateSearchAnimation();
}

//プロパティ一覧
void BowTower::RegisterProperties(PropertyRegistry* registry) {
	Component::RegisterProperties(registry);
	registry->Add("探索範囲の最大角度",maxHeadRotate_);
	registry->Add("探索範囲の最小角度",minHeadRotate_);
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

//探してる時のアニメーション
void BowTower::UpdateSearchAnimation() {
	//オフセットを補間
	offset += 
	//頭のTransformを取得
	Object3d* object3d = GetOwner()->GetComponent<Object3d>();
	if(!object3d) {
		Logger::OutputLog("Object3dが取得できませんでした");
		return;
	}
	Transform nodeTransform = object3d->GetNodeLocalTransform("BowTower/head");
	Vector3 rotate = nodeTransform.GetDegreeAngle();
	nodeTransform.SetEulerAngleDegrees({rotate.x,rotate.y + offset,rotate.z});

	object3d->SetNodeLocalTransform("BowTower/head",nodeTransform);
}
