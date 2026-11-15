#include "FollowCameraController.h"

//コンストラクタ
FollowCameraController::FollowCameraController(GameObject* gameObject) :Component(gameObject) {
}

//デストラクタ
FollowCameraController::~FollowCameraController() {
}

//初期化
void FollowCameraController::Initialize() {
	spherical.radius = 6.0f;
	spherical.theta = 0.0f;
	spherical.phi = -mathUtility::kPi/2.0f;
}

//更新
void FollowCameraController::Update() {
}

//複製
std::unique_ptr<Component> FollowCameraController::Clone(GameObject* gameObject) const {
	std::unique_ptr<FollowCameraController>cloneInstance = std::make_unique<FollowCameraController>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	return cloneInstance;
}

//コンポーネントのタイプ名を返す
std::string FollowCameraController::GetTypeName() const {
	return "FollowCameraController";
}

//プロパティ一覧を登録
void FollowCameraController::RegisterProperties(PropertyRegistry* registry) {
}
