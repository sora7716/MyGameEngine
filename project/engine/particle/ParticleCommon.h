#pragma once
#pragma once
#include "RenderData.h"
#include "blendMode.h"
#include <wrl.h>
#include <d3d12.h>
#include <array>
//前方宣言
class DirectXBase;
class SRVManager;
class TextureManager;
class GraphicsPipeline;
class Camera;
class Blend;

/// <summary>
/// パーティクルの共通部分
/// </summary>
class ParticleCommon {
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~ParticleCommon();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤</param>
	/// <param name="srvManager">SRVマネージャー</param>
	/// <param name="textureManager">テクスチャマネージャー</param>
	void Initialize(DirectXBase* directXBase, SRVManager* srvManager, TextureManager* textureManager);

	/// <summary>
	/// 共通描画設定
	/// </summary>
	void DrawSetting();

	/// <summary>
	/// DirectionalLightのリソースのゲッター
	/// </summary>
	/// <returns>DirectionalLightのリソース</returns>
	//ID3D12Resource* GetDirectionalLightResource()const;

	/// <summary>
	/// DirectXの基盤のゲッター
	/// </summary>
	/// <returns>DirectXの基盤</returns>
	DirectXBase* GetDirectXBase()const;

	/// <summary>
	/// SRVマネージャーのゲッター
	/// </summary>
	/// <returns>SRVマネージャー</returns>
	SRVManager* GetSRVManager()const;

	/// <summary>
	/// テクスチャマネージャーのゲッター
	/// </summary>
	/// <returns>テクスチャマネージャー</returns>
	TextureManager* GetTextureManager()const;

	/// <summary>
	/// グラフィックパイプラインのゲッター
	/// </summary>
	/// <returns>グラフィックパイプライン</returns>
	std::array<ComPtr<ID3D12PipelineState>, static_cast<int32_t>(BlendMode::kCountOfBlendMode)>GetGraphicsPipelineStates()const;

	/// <summary>
	/// デフォルトカメラのセッター
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetDefaultCamera(Camera* camera);

	/// <summary>
	/// デフォルトカメラのゲッター
	/// </summary>
	/// <returns>デフォルトカメラ</returns>
	Camera* GetDefaultCamera()const;
public://PassKey
	class ConstructorKey {
	private:
		ConstructorKey() = default;
		friend class Core;
	};
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PassKeyを受け取る</param>
	explicit ParticleCommon(ConstructorKey);
private://メンバ関数
	//コピーコンストラクタ禁止
	ParticleCommon(const ParticleCommon&) = delete;
	//代入演算子の禁止
	ParticleCommon operator=(const ParticleCommon&) = delete;
private://静的メンバ変数
	//インスタンス
	static inline ParticleCommon* instance = nullptr;
	//Finalizeをしたかどうかのフラグ
	static inline bool isFinalize = false;
private://メンバ変数
	//DirectXの基盤
	DirectXBase* directXBase_ = nullptr;
	//SRVマネージャー
	SRVManager* srvManager_ = nullptr;
	//ルートシグネイチャ
	ComPtr<ID3D12RootSignature>rootSignature_ = nullptr;
	//グラフィックスパイプライン(PSO)
	std::array<ComPtr<ID3D12PipelineState>, static_cast<int32_t>(BlendMode::kCountOfBlendMode)> graphicsPipelineStates_ = { nullptr };
	//グラフィックスパイプライン
	GraphicsPipeline* makeGraphicsPipeline_ = nullptr;
	//テクスチャマネージャー
	TextureManager* textureManager_ = nullptr;
	//ブレンド
	Blend* blend_ = nullptr;
	BlendMode blendMode_ = BlendMode::kNone;
	//デフォルトカメラ
	Camera* defaultCamera_ = nullptr;
};
