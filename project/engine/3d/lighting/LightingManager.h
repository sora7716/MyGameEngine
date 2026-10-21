#pragma once
#include "LightingData.h"
#include <wrl.h>
#include <d3d12.h>
#include <vector>
#include <memory>

//前方宣言
class Core;
class DirectXBase;
class SRVManager;
class GameObject;

/// <summary>
/// ライティングの管理
/// </summary>
class LightingManager{
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
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
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="srvManager">SRVの管理</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<LightingManager>Create(ConstructorKey key, DirectXBase* directXBase, SRVManager* srvManager);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PassKeyを受け取る</param>
	explicit LightingManager(ConstructorKey);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~LightingManager();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="srvManager">SRVの管理</param>
	void Initialize(DirectXBase* directXBase, SRVManager* srvManager);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画の設定
	/// </summary>
	void Bind();

	/// <summary>
	/// 有効な平行光源を集める
	/// </summary>
	/// <param name="gameObjects">ゲームオブジェクトの配列</param>
	void CollectActiveDirectionalLight(const std::vector<std::unique_ptr<GameObject>>& gameObjects);

	/// <summary>
	/// 有効な点光源を集める
	/// </summary>
	/// <param name="gameObjects">ゲームオブジェクトの配列</param>
	void CollectActivePointLight(const std::vector<std::unique_ptr<GameObject>>& gameObjects);
private://メンバ関数
	//コピーコンストラクタ禁止
	LightingManager(const LightingManager&) = delete;
	//代入演算子の禁止
	LightingManager& operator=(const LightingManager&) = delete;

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
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//SRVManagerの管理
	SRVManager* srvManager_ = nullptr;
	//バッファリソース
	ComPtr<ID3D12Resource> directionalLightResource_ = nullptr;//平行光源
	ComPtr<ID3D12Resource> pointLightResource_ = nullptr;//点光源
	ComPtr<ID3D12Resource> spotLightResource_ = nullptr;//スポットライト

	//バッファリソース内のデータを指すポインタ
	lightData::DirectionalLight* directionalLight_ = nullptr;//平行光源
	std::vector<lightData::PointLight> pointLights_;//点光源
	std::vector<lightData::SpotLight> spotLights_;//スポットライト

	//SRVインデックス
	uint32_t srvIndexPoint_ = 0;//PointLight
	uint32_t srvIndexSpot_ = 0;//SpotLight

	//マップしたポインタ
	lightData::PointLight* mappedPointLightData_ = nullptr;
};

