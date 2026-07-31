#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <vector>

//前方宣言
class DirectXBase;
class SRVManager;
class Camera;
class Model;

/// <summary>
/// Object3dの描画を担当
/// </summary>
class Object3dRenderer{
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://構造体など
	struct LODRenderData{
		std::vector<Model*>models = {};
		std::vector<uint32_t>srvIndices = {};
		std::vector<uint32_t>drawCounts = {};

		/// <summary>
		/// サイズがあっている確認
		/// </summary>
		/// <returns>サイズがあっているか</returns>
		bool IsLodCountValid()const;
	};

	//描画に必要なデータ
	struct RenderData{
		ComPtr<ID3D12Resource>directionalLightResource = nullptr;
		uint32_t pointLightSrvIndex = 0;
		uint32_t spotLightSrvIndex = 0;
		Camera* renderCamera = nullptr;
		LODRenderData lodRenderData = {};

	};
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Object3dRenderer();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Object3dRenderer();


	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="srvManager">SRVの管理</param>
	/// <param name="maxInstance">インスタンスの最大値</param>
	void Initialize(DirectXBase* directXBase, SRVManager* srvManager, uint32_t maxInstance = 1024);

	/// <summary>
	///リセット
	/// </summary>
	void Reset();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 描画データの追加
	/// </summary>
	/// <param name="renderData">描画データ</param>
	void AddRenderData(const RenderData& renderData);
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//SRVの管理
	SRVManager* srvManager_ = nullptr;
	//インスタンスの最大値
	uint32_t maxInstanceCount_ = 0;
	//描画データ
	std::vector<RenderData>renderDatas_;
};

