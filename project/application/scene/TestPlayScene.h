#pragma once
#include "BaseScene.h"
#include "ColliderData.h"
#include "LightingData.h"
#include "Input.h"
#include <vector>

//前方宣言
class Object3d;
class GameObject;
class Audio;
class SkyBox;
class ParticleSystem;

namespace debugDraw {
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
	void Initialize(const SceneContext& sceneContext, Object3dRenderer* object3dRenderer, SkyBoxRenderer* skyBoxRenderer, DebugDrawRenderer* debugDrawRenderer)override;

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
	/// <param name="camera">使用するカメラ</param>
	void Draw(Camera*camera)override;

	/// <summary>
	/// デバッグでの描画
	/// </summary>
	void DebugDraw()override;

	/// <summary>
	/// ゲームでの描画
	/// </summary>
	void GameDraw()override;

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize()override;
private://メンバ変数
	std::unique_ptr<Object3d>object3d_ = nullptr;
	float environmentCoefficient_ = 1.0f;

	std::vector<Transform2d>transform2ds_;

	std::unique_ptr<debugDraw::Frustum>frustum_ = nullptr;

	std::unique_ptr<debugDraw::Cube>cube_ = nullptr;

	std::unique_ptr<SkyBox>skyBox_ = nullptr;

	std::unique_ptr<ParticleSystem>particleSystem_ = nullptr;
	Vector3 emitterPos_ = {};

	DirectionalLight directionalLight_ = {};
};
