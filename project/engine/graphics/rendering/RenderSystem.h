#pragma once
#include "PipelineManagerData.h"
#include <memory>

//前方宣言
class DirectXBase;
class SRVManager;
class TextureManager;
class PipelineManager;
class LightingManager;
class Blend;
class Object3dRenderer;
class SkyBoxRenderer;

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
	/// Object3dのレンダラーの取得
	/// </summary>
	/// <returns>Object3dのレンダラー</returns>
	Object3dRenderer* GetObject3dRenderer();

	/// <summary>
	/// SkyBoxのレンダラーの取得
	/// </summary>
	/// <returns>SkyBoxのレンダラー</returns>
	SkyBoxRenderer* GetSkyBoxRenderer();
private://メンバ関数
	/// <summary>
	/// 描画の開始
	/// </summary>
	/// <param name="blendMode">ブレンドモード</param>
	/// <param name="pipelineType">パイプラインモード</param>
	void PreDraw(BlendMode blendMode, PipelineType pipelineType);
private://メンバ関数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//パイプラインの管理
	PipelineManager* pipelineManager_ = nullptr;
	//ライティングの管理
	LightingManager* lightingManager_ = nullptr;
	//Object3dのレンダラー
	std::unique_ptr<Object3dRenderer>object3dRenderer_ = nullptr;
	//スカイボックスのレンダラー
	std::unique_ptr<SkyBoxRenderer>skyBoxRenderer_ = nullptr;
};

