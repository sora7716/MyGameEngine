#pragma once
#include "RenderData.h"
#include "blendMode.h"
#include <wrl.h>
#include <d3d12.h>
#include <array>
#include <string>
//前方宣言
class DirectXBase;
class TextureManager;
class Blend;
class GraphicsPipeline;

/// <summary>
/// スプライトの共通部分
/// </summary>
class SpriteCommon {
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~SpriteCommon();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="textureManager">テクスチャマネージャー</param>
	void Initialize(DirectXBase* directXBase,TextureManager*textureManager);

	/// <summary>
	/// 共通描画設定
	/// </summary>
	void DrawSetting();

	/// <summary>
	/// テクスチャの読み込み
	/// </summary>
	/// <param name="filename">ファイル名</param>
	void LoadTexture(std::string& filename);

	/// <summary>
	/// 平行光源の生成
	/// </summary>
	void CreateDirectionLight();

	/// <summary>
	/// 点光源
	/// </summary>
	void CreatePointLight();

	/// <summary>
	/// DirectionalLightのセッター
	/// </summary>
	/// <param name="directionalLightData">DirectionalLightデータ</param>
	void SetDirectionalLightData(const DirectionalLight& directionalLightData);

	/// <summary>
	/// DirectionalLightのリソースのゲッター
	/// </summary>
	/// <returns>DirectionalLightのリソース</returns>
	ID3D12Resource* GetDirectionalLightResource()const;

	/// <summary>
	/// PointLightのリソースのゲッター
	/// </summary>
	/// <returns>PointLightのリソース</returns>
	ID3D12Resource* GetPointLightResource()const;

	/// <summary>
	/// DirectXの基盤部分のゲッター
	/// </summary>
	/// <returns></returns>
	DirectXBase* GetDirectXBase()const;

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
public://PressKeyIdiom
	class ConstructorKey {
	private:
		ConstructorKey() = default;
		friend class Core;
	};
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PressKeyを受け取る</param>
	explicit SpriteCommon(ConstructorKey);
private://メンバ関数
	//コピーコンストラクタ禁止
	SpriteCommon(const SpriteCommon&) = delete;
	//代入演算子の禁止
	SpriteCommon operator=(const SpriteCommon&) = delete;
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//ルートシグネイチャ
	ComPtr<ID3D12RootSignature>rootSignature_ = nullptr;
	//グラフィックスパイプライン(PSO)
	std::array<ComPtr<ID3D12PipelineState>, static_cast<int32_t>(BlendMode::kCountOfBlendMode)> graphicsPipelineStates_ = { nullptr };
	//グラフィックスパイプライン
	GraphicsPipeline* makeGraphicsPipeline_ = nullptr;
	
	//バッファリソース
	ComPtr<ID3D12Resource> directionalLightResource_ = nullptr;//平行光源
	ComPtr<ID3D12Resource> pointLightResource_ = nullptr;//点光源
	//バッファリソース内のデータを指すポインタ
	DirectionalLight* directionalLightPtr_ = nullptr;//平行光源
	PointLight* pointLightPtr_ = nullptr;//点光源

	//テクスチャマネージャー
	TextureManager* textureManager_ = nullptr;

	//ブレンド
	Blend* blend_ = nullptr;
};