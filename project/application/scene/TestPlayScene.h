#pragma once
#include "BaseScene.h"
#include "GameObjectData.h"
#include "ColliderData.h"
#include "Input.h"
#include <vector>

//前方宣言
class Camera;
class Object3d;
class Line;
class Cube;
class Circle;
class BaseShape;

/// <summary>
/// テストプレイシーン
/// </summary>
class TestPlayScene :public BaseScene {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	TestPlayScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TestPlayScene()override;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="sceneContext">シーンで必要なもの</param>
	void Initialize(const SceneContext& sceneContext)override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug()override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw()override;

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize()override;
private://メンバ変数
	Camera* camera_ = nullptr;
	//Xboxの番号
	DWORD xBoxPadNumber_ = 0;
	//オブジェクト3d
	std::unique_ptr<Object3d>object3d_ = nullptr;
	std::vector<GameObject> gameObject_ = {};
	PhysicsData physicsData_ = {};
	Vector3 scale = Vector3::MakeAllOne();

	Quaternion end = { 0.5f,0.8f,0.0f,0.2f };
	Quaternion start = Quaternion::IdentityQuaternion();
	float frame_ = 0.0f;
	bool isAnimation_ = false;
	Vector3 eulerAngle_ = {};
	Vector3 axis_ = { 0.0f,1.0f,0.0f };
	float angle_ = 0.0f;

	std::vector<ColliderState> colliderStates_ = {};
	std::vector<Collider> colliders_ = {};

	std::unique_ptr<BaseShape>shape_ = nullptr;
	std::unique_ptr<Line>line_ = nullptr;
	PrimitiveData::Segment segment_ = { {0.0f,0.0f,0.0f},{0.5f,0.0f,0.0f} };

	std::unique_ptr<Cube>cube_ = nullptr;
	PrimitiveData::OBB obb_ = {};

	std::unique_ptr<Circle>circle_ = nullptr;
};
