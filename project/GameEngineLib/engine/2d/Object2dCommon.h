#pragma once
#include "ResourceData.h"
#include "blendMode.h"
#include <wrl.h>
#include <d3d12.h>
#include <array>
//前方宣言
class DirectXBase;
class TextureManager;
class Camera;
class GraphicsPipeline;
class Blend;

/// <summary>
/// 2Dオブジェクトの共通部分
/// </summary>
class Object2dCommon {
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Object2dCommon();
	
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤</param>
	/// <param name="textureManager">テクスチャマネージャー</param>
	void Initialize(DirectXBase* directXBase, TextureManager* textureManager);

	/// <summary>
	/// 共通描画設定
	/// </summary>
	void DrawSetting();

	/// <summary>
	/// 光源の生成
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
	/// DirectXの基盤のゲッター
	/// </summary>
	/// <returns>DirectXの基盤</returns>
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
	explicit Object2dCommon(ConstructorKey);
private://メンバ関数
	//コピーコンストラクタ禁止
	Object2dCommon(const Object2dCommon&) = delete;
	//代入演算子の禁止
	Object2dCommon operator=(const Object2dCommon&) = delete;
private://静的メンバ変数
	//インスタンス
	static inline Object2dCommon* instance = nullptr;
	//Finalizeをしたかどうかのフラグ
	static inline bool isFinalize = false;
private://メンバ変数
	//DirectXの基盤
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
	BlendMode blendMode_ = BlendMode::kNone;
	//デフォルトカメラ
	Camera* defaultCamera_ = nullptr;
};

