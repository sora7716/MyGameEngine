#pragma once
#include "Framework.h"
#include "Context.h"

//前方宣言
class RenderTexture;

/// <summary>
/// ゲームシステム
/// </summary>
class GameSystem :public Framework {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameSystem();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameSystem();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

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
	//レンダーテクスチャで必要なもの
	RenderTextureContext renderTextureContext_ = {};
	//シーンのレンダーテクスチャ
	std::unique_ptr<RenderTexture>sceneRenderTexture_ = nullptr;
	//プレビューのレンダーテクスチャ
	std::unique_ptr<RenderTexture>previewRenderTexture_ = nullptr;
};

