#include "Culling.h"
#include "Collision.h"
#include "Camera.h"
#include <cassert>

//生成
std::unique_ptr<Culling> Culling::Create(Camera* camera) {
	//インスタンスの生成
	std::unique_ptr<Culling>instance = std::make_unique<Culling>();
	//初期化
	instance->Initialize(camera);

	return std::move(instance);
}

//コンストラクタ
Culling::Culling() {
}

//デストラクタ
Culling::~Culling() {
}

//初期化
void Culling::Initialize(Camera* camera) {
	//カメラが存在するか
	assert(camera);
	//カメラを記録
	camera_ = camera;
}

//視錐台カリングをするか
bool Culling::IsVisibleInFrustum(const PrimitiveData::AABB& aabb, const Matrix4x4& worldMatrix) {
	//カメラがなかった場合
	if (!camera_) {
		return false;
	}

	//表示するかのフラグ
	bool isVisible = false;

	//ワールド座標でのAABBを取得
	PrimitiveData::AABB worldAABB = aabb * worldMatrix;

	//衝突しているかどうか
	if (Collision::IsCollision(camera_->GetFrustum(), worldAABB)) {
		isVisible = true;
	}

	return isVisible;
}
