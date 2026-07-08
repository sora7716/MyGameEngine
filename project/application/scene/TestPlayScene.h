#pragma once
#include "BaseScene.h"
#include "ColliderData.h"
#include "Input.h"
#include <vector>

//前方宣言
class Object3d;
class GameObject;
class Audio;
class SkyBox;

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
	std::unique_ptr<Object3d>object3d_ = nullptr;
	std::vector<std::unique_ptr<GameObject>>gameObjects_;

	std::vector<Transform2d>transform2ds_;

	std::unique_ptr<Primitive::Frustum>frustum_ = nullptr;

	std::unique_ptr<Primitive::Cube>cube_ = nullptr;

	std::unique_ptr<SkyBox>skyBox_ = nullptr;
};
