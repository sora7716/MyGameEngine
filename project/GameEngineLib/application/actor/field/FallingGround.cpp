#include "FallingGround.h"
#include "algorithms/Physics.h"
#include "algorithms/Math.h"
#include "Object3d.h"
#include "ImGuiManager.h"

//コンストラクタ
FallingGround::FallingGround() {
}

//デストラクタ
FallingGround::~FallingGround() {
}

//初期化
void FallingGround::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
	//オブジェクトの数とモデルを設定
	entityGroup_.objectCount = 2;
	entityGroup_.modelName = "cube";
	//基底クラスの初期化
	BaseGround::Initialize(object3dCommon, camera);
	//位置を調整
	entityGroup_.entity[0].gameObject.transformData.translate = { 9.0f,7.5f,18.0f };
	entityGroup_.entity[1].gameObject.transformData.translate = { 6.2f,8.1f,13.5f };
}

//更新
void FallingGround::Update() {
	if (isFalling_) {
		//落ちるまで計測
		if (fallDelaySecond_ > 0.0f) {
			fallDelaySecond_ -= Math::kDeltaTime;
		} else {
			entityGroup_.entity[fallingBlockIndex_].physicsData.acceleration = Physics::kGravity;
			//落ちるフラグと秒数を初期化
			isFalling_ = false;
			fallDelaySecond_ = 0.0f;
		}
	}
	//基底クラスの更新
	BaseGround::Update();
}

//デバッグ
void FallingGround::Debug() {
#ifdef USE_IMGUi
	//基底クラスのデバッグ
	BaseGround::Debug();
	ImGui::Text("fallingCount:%f", fallDelaySecond_);
#endif // USE_IMGUi
}

//描画
void FallingGround::Draw() {
	//基底クラスの描画
	BaseGround::Draw();
}

//衝突したら
void FallingGround::OnCollision(uint32_t index, ColliderState* other) {
	if (*other->tagPtr == Tag::kPlayer) {
		if (!isFalling_) {
			fallDelaySecond_ = kFallDelaySecond;//落ちるまでの時間を設定
			isFalling_ = true;//落ちるフラグを立てる
		}
		fallingBlockIndex_ = index;//落ちるブロック番号を保存
	}
}
