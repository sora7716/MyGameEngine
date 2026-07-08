#pragma once
#include "RenderData.h"
#include "blendMode.h"
#include <wrl.h>
#include <d3d12.h>
#include <array>
#include <vector>

//前方宣言
class DirectXBase;
class SRVManager;
class GraphicsPipeline;
class TextureManager;
class ModelManager;
class Camera;
class Blend;

/// <summary>
/// 3Dオブジェクトの共通部分
/// </summary>
class Object3dCommon {
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Object3dCommon();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤</param>
	/// <param name="srvManager">srvマネージャー</param>
	/// <param name="textureManager">テクスチャマネージャー</param>
	/// <param name="modelManager">モデルマネージャー/param>
	void Initialize(DirectXBase* directXBase, SRVManager* srvManager, TextureManager* textureManager, ModelManager* modelManager);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 共通描画設定
	/// </summary>
	void DrawSetting();

	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug();

	/// <summary>
	/// カメラリソースの生成
	/// </summary>
	void CreateCameraResource(const Vector3& cameraTranslate);

	/// <summary>
	/// カメラの位置のセッター
	/// </summary>
	/// <param name="cameraTranslate"></param>
	void SetCameraForGPU(const Vector3& cameraTranslate);

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
	/// SpotLightのリソースのゲッター
	/// </summary>
	/// <returns>SpotLightのリソース</returns>
	ID3D12Resource* GetSpotLightResource()const;

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
	/// モデルマネージャーのゲッター
	/// </summary>
	/// <returns>モデルマネージャー</returns>
	ModelManager* GetModelManager()const;

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

	/// <summary>
	/// SRVインデックスのゲッター(PointLight)
	/// </summary>
	/// <returns>SRVインデックス</returns>
	uint32_t GetSrvIndexPoint()const;

	/// <summary>
	/// SRVインデックスのゲッター(SpotLight)
	/// </summary>
	/// <returns>SRVインデックス</returns>
	uint32_t GetSrvIndexSpot()const;

	/// <summary>
	/// 平行光源のゲッター
	/// </summary>
	/// <returns>平行光源</returns>
	const DirectionalLight& GetDirectionalLight()const;

	/// <summary>
	/// 点光源のセッター
	/// </summary>
	/// <returns>点光源</returns>
	PointLight* GetPointLight();

	/// <summary>
	/// スポットライトのゲッター
	/// </summary>
	/// <returns>スポットライト</returns>
	SpotLight* GetSpotLightPtr();

	/// <summary>
	/// スポットライトのセッター
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <param name="spotLight">スポットライト</param>
	void SetSpotLightList(uint32_t index, const SpotLight& spotLight);

	/// <summary>
	/// ポイントライトの位置のセッター
	/// </summary>
	/// <param name="pointLightPos">ポイントライトの位置</param>
	void SetPointLightPos(const Vector3& pointLightPos);
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
	explicit Object3dCommon(ConstructorKey);
private://メンバ関数
	//コピーコンストラクタ禁止
	Object3dCommon(const Object3dCommon&) = delete;
	//代入演算子の禁止
	Object3dCommon operator=(const Object3dCommon&) = delete;

	/// <summary>
	/// 平行光源の生成
	/// </summary>
	void CreateDirectionLight();

	/// <summary>
	/// 点光源の生成
	/// </summary>
	void CreatePointLight();

	/// <summary>
	/// 点光源のストラクチャバッファの生成
	/// </summary>
	void CreateStructuredBufferForPoint();

	/// <summary>
	/// スポットライトの生成
	/// </summary>
	void CreateSpotLight();

	/// <summary>
	/// スポットライトのストラクチャバッファの生成
	/// </summary>
	void CreateStructuredBufferForSpot();
private://静的メンバ変数
	//ライトの最大値
	static inline const int32_t kMaxLightCount = 64;
private://メンバ変数
	//DirectXの基盤
	DirectXBase* directXBase_ = nullptr;

	//SRVマネージャー
	SRVManager* srvManager_ = nullptr;

	//モデルマネージャー
	ModelManager* modelManager_ = nullptr;

	//ルートシグネイチャ
	ComPtr<ID3D12RootSignature>rootSignature_ = nullptr;

	//グラフィックスパイプライン(PSO)
	std::array<ComPtr<ID3D12PipelineState>, static_cast<int32_t>(BlendMode::kCountOfBlendMode)> graphicsPipelineStates_ = { nullptr };
	//グラフィックスパイプライン
	GraphicsPipeline* makeGraphicsPipeline_ = nullptr;

	//テクスチャマネジャー
	TextureManager* textureManager_ = nullptr;

	//バッファリソース
	ComPtr<ID3D12Resource> directionalLightResource_ = nullptr;//平行光源
	ComPtr<ID3D12Resource> pointLightResource_ = nullptr;//点光源
	ComPtr<ID3D12Resource> spotLightResource_ = nullptr;//スポットライト
	ComPtr<ID3D12Resource> cameraResource_ = nullptr;//カメラ
	//バッファリソース内のデータを指すポインタ
	DirectionalLight* directionalLightPtr_ = nullptr;//平行光源
	PointLight* pointLightPtr_ = nullptr;//点光源
	SpotLight* spotLightPtr_ = nullptr;//スポットライト
	CameraForGPU* cameraForGPU_ = nullptr;//カメラ

	//平行光源
	DirectionalLight directionalLightData_ = {};
	//点光源
	PointLight pointLightDataList_[kMaxLightCount] = {};
	//スポットライト
	std::vector<SpotLight>spotLightList_ = {};

	//ブレンド
	Blend* blend_ = nullptr;
	BlendMode blendMode_ = BlendMode::kNone;

	//デフォルトカメラ
	Camera* defaultCamera_ = nullptr;

	//SRVインデックス
	uint32_t srvIndexPoint_ = 0;//PointLight
	uint32_t srvIndexSpot_ = 0;//SpotLight

	//光源(追従するターゲット)
	Vector3 pointLightPos_ = {};
};
