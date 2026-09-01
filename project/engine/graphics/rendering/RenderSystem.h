#pragma once
#include "PipelineManagerData.h"
#include "RenderingData.h"
#include <vector>
#include <memory>

//前方宣言
class DirectXBase;
class SRVManager;
class TextureManager;
class PipelineManager;
class LightingManager;
class Camera;
class GameObject;
class Model;
class Object3d;
class Object3dRenderer;
class MaterialInstance;
class SkyBox;
class SkyBoxRenderer;
class Sprite;
class SpriteRenderer;
namespace debugDraw{
	class BaseShape;
}
class DebugDrawRenderer;
class ParticleSystem;
class ParticleRenderer;

//描画グループごとにObject3dを分ける
struct Object3dBatch{
	Model* model = nullptr;
	MaterialInstance* materialInstance = nullptr;
	BlendMode blendMode = BlendMode::kNone;

	std::vector<Object3d*>instances;
	//GPUへ送るインスタンスごとの行列
	std::vector<TransformationMatrix>transformations;
};

/// <summary>
/// 描画のシステム
/// </summary>
class RenderSystem{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	RenderSystem();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~RenderSystem();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤</param>
	/// <param name="srvManager">SRVの管理</param>
	/// <param name="textureManager">Textureの管理</param>
	/// <param name="pipelineManager">パイプラインの管理</param>
	/// <param name="lightingManager">ライティングの管理</param>
	void Initialize(DirectXBase* directXBase, SRVManager* srvManager, TextureManager* textureManager, PipelineManager* pipelineManager, LightingManager* lightingManager);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 描画に有効なObject3dを集める
	/// </summary>
	/// <param name="gameObjects">ゲームオブジェクトの配列</param>
	/// <param name="renderCamera">描画で使用するカメラ</param>
	void CollectActiveObject3ds(const std::vector<std::unique_ptr<GameObject>>& gameObjects, Camera* renderCamera);

	/// <summary>
	/// 描画に有効なSkyBoxを集める
	/// </summary>
	/// <param name="gameObjects">ゲームオブジェクトの配列</param>
	/// <param name="renderCamera">描画で使用するカメラ</param>
	void CollectActiveSkyBox(const std::vector<std::unique_ptr<GameObject>>& gameObjects, Camera* renderCamera);

	/// <summary>
	/// 描画に有効なSpriteを集める
	/// </summary>
	/// <param name="gameObjects">ゲームオブジェクトの配列</param>
	void CollectActiveSprites(const std::vector<std::unique_ptr<GameObject>>& gameObjects);

	/// <summary>
	/// 描画に有効なDebugDrawを集める
	/// </summary>
	/// <param name="gameObjects">ゲームオブジェクトの配列</param>
	/// <param name="renderCamera">描画で使用するカメラ</param>
	void CollectActiveDebugDraw(const std::vector<std::unique_ptr<GameObject>>& gameObjects, Camera* renderCamera);

	/// <summary>
	/// 描画に有効なパーティクルシステムを集める
	/// </summary>
	/// <param name="gameObjects">ゲームオブジェクトの配列</param>
	/// <param name="renderCamera">描画で使用するカメラ</param>
	void CollectActiveParticleSystems(const std::vector<std::unique_ptr<GameObject>>& gameObjects, Camera* renderCamera);
private://メンバ関数
	/// <summary>
	/// 描画の開始
	/// </summary>
	/// <param name="blendMode">ブレンドモード</param>
	/// <param name="pipelineType">パイプラインモード</param>
	void PreDraw(BlendMode blendMode, PipelineType pipelineType);

	/// <summary>
	/// 描画開始
	/// </summary>
	/// <param name="blendMode">ブレンドモード</param>
	void PreDraw(BlendMode blendMode);

	/// <summary>
	/// オブジェクト3dの描画グループを構築
	/// </summary>
	void BuildObject3dBatches();

	/// <summary>
	/// トランスフォーメーションデータの構築
	/// </summary>
	void BuildTransformationData();

	/// <summary>
	/// オブジェクト3dのバッチをレンダラーの送る
	/// </summary>
	void SubmitObject3dBatches();
private://メンバ関数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;

	//パイプラインの管理
	PipelineManager* pipelineManager_ = nullptr;

	//ライティングの管理
	LightingManager* lightingManager_ = nullptr;

	//カメラ
	Camera* renderCamera_ = nullptr;

	//描画に有効なObject3d
	std::vector<Object3d*>activeObject3ds_;
	//Object3dを描画グループごとに分ける
	std::vector<Object3dBatch>object3dBatches_;
	//Object3dのレンダラー
	std::unique_ptr<Object3dRenderer>object3dRenderer_ = nullptr;

	//描画に有効なSkyBox
	SkyBox* activeSkyBox_ = nullptr;
	//スカイボックスのレンダラー
	std::unique_ptr<SkyBoxRenderer>skyBoxRenderer_ = nullptr;

	//描画に有効なSprite
	std::vector<Sprite*>activeSprites_;
	//スプライトのレンダラー
	std::unique_ptr<SpriteRenderer>spriteRenderer_ = nullptr;

	//描画に有効なDebugDraw
	std::vector<debugDraw::BaseShape*>activeDebugDraws_;
	//DebugDrawのレンダラー
	std::unique_ptr<DebugDrawRenderer>debugDrawRenderer_ = nullptr;

	//描画に有効なパーティクルシステム
	std::vector<ParticleSystem*>activeParticleSystems_;
	//パーティクルの描画のレンダラー
	std::unique_ptr<ParticleRenderer>particleRenderer_ = nullptr;
};

