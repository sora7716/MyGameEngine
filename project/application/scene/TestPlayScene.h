#pragma once
#include "BaseScene.h"
#include "GameObjectData.h"
#include "ColliderData.h"
#include "Input.h"
#include <vector>

//前方宣言
class Box;
class Object3d;

namespace Primitive {
	class Cube;
	class Frustum;
	class Line;
	class Plane;
	class Sphere;
}


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
	std::unique_ptr<Box>box_ = nullptr;

	std::unique_ptr<Object3d>object3d_ = nullptr;

	std::vector<Transform2dData>transform2ds_;

	std::unique_ptr<Primitive::Frustum>frustum_ = nullptr;

	Camera* testPlayCamera = nullptr;

	std::unique_ptr<Primitive::Plane>plane_ = nullptr;

	std::unique_ptr<Primitive::Sphere>sphere_ = nullptr;
};
