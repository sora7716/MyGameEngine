#pragma once
#include "PipelineManagerData.h"
#include "RenderingData.h"
#include "CameraRenderData.h"
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
class Camera;
class CameraRenderer;

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
private://構造体
	//選択するカメラ
	struct SelectCamera{
		Camera* camera = nullptr;
		uint32_t index;
	};
public://PassKey
	class ConstructorKey{
	private:
		ConstructorKey() = default;
		friend class Core;
	};
public://静的メンバ関数
	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="key">コンストラクタのKey</param>
	/// <param name="directXBase">DirectXの基盤</param>
	/// <param name="srvManager">SRVの管理</param>
	/// <param name="textureManager">Textureの管理</param>
	/// <param name="pipelineManager">パイプラインの管理</param>
	/// <param name="lightingManager">ライティングの管理</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<RenderSystem>Create(ConstructorKey key, DirectXBase* directXBase, SRVManager* srvManager, TextureManager* textureManager, PipelineManager* pipelineManager, LightingManager* lightingManager);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PassKeyを受け取る</param>
	explicit RenderSystem(ConstructorKey);

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
	void CollectActiveObject3ds(const std::vector<std::unique_ptr<GameObject>>& gameObjects);

	/// <summary>
	/// 描画に有効なSkyBoxを集める
	/// </summary>
	/// <param name="gameObjects">ゲームオブジェクトの配列</param>
	void CollectActiveSkyBox(const std::vector<std::unique_ptr<GameObject>>& gameObjects);

	/// <summary>
	/// 描画に有効なSpriteを集める
	/// </summary>
	/// <param name="gameObjects">ゲームオブジェクトの配列</param>
	void CollectActiveSprites(const std::vector<std::unique_ptr<GameObject>>& gameObjects);

	/// <summary>
	/// 描画に有効なDebugDrawを集める
	/// </summary>
	/// <param name="gameObjects">ゲームオブジェクトの配列</param>
	void CollectActiveDebugDraw(const std::vector<std::unique_ptr<GameObject>>& gameObjects);

	/// <summary>
	/// 描画に有効なパーティクルシステムを集める
	/// </summary>
	/// <param name="gameObjects">ゲームオブジェクトの配列</param>
	void CollectActiveParticleSystems(const std::vector<std::unique_ptr<GameObject>>& gameObjects);

	/// <summary>
	/// 描画に有効なカメラを集める
	/// </summary>
	/// <param name="gameObjects">ゲームオブジェクトの配列</param>
	void CollectActiveCameras(const std::vector<std::unique_ptr<GameObject>>& gameObjects);

	/// <summary>
	/// カメラモードの設定
	/// </summary>
	/// <param name="cameraMode">カメラモード</param>
	/// <returns>セレクトカメラが取得できたか</returns>
	bool SetCameraMode(CameraMode cameraMode);
private://メンバ関数
	//コピーコンストラクタ禁止
	RenderSystem(const RenderSystem&) = delete;
	//代入演算子の禁止
	RenderSystem operator=(const RenderSystem&) = delete;

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

	/// <summary>
	/// セレクトカメラの取得
	/// </summary>
	/// <param name="cameraMode">カメラモード</param>
	/// <returns>セレクトカメラを取得できたか</returns>
	bool FindSelectCamera(CameraMode cameraMode);
private://メンバ関数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;

	//パイプラインの管理
	PipelineManager* pipelineManager_ = nullptr;

	//ライティングの管理
	LightingManager* lightingManager_ = nullptr;

	//カメラモード
	CameraMode cameraMode_ = CameraMode::kMain;
	SelectCamera selectCamera_ = {};

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

	//描画に有効なカメラ
	std::vector<Camera*> activeCameras_;
	//カメラの描画レンダラー
	std::unique_ptr<CameraRenderer>cameraRenderer_ = nullptr;
};

