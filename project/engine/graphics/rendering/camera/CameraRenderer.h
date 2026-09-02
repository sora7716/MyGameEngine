#pragma once
#include "CameraRenderData.h"
#include <memory>
#include <cstdint>
#include <vector>
#include <wrl.h>
#include <d3d12.h>

//前方宣言
class DirectXBase;

/// <summary>
/// カメラの描画を担当
/// </summary>
class CameraRenderer{
public://静的メンバ関数
	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<CameraRenderer>Create(DirectXBase* directXBase);
private://構造体
	//カメラのデータの構造体
	struct CameraForGPU{
		Vector3 worldPosition = {};
		float padding = 0.0f;
		Matrix4x4 viewProjection = Matrix4x4::Identity4x4();
	};

	//GPUリソース
	struct GpuResource{
		CameraForGPU* cameraForGPU = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource = nullptr;
	};
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	CameraRenderer();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~CameraRenderer();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	void Initialize(DirectXBase* directXBase);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="instanceIndex">インスタンスの検索キー</param>
	/// <param name="renderCamera">描画用のカメラ</param>
	void Bind(uint32_t instanceIndex, uint32_t parameterIndex);

	/// <summary>
	/// リセット
	/// </summary>
	void Reset();

	/// <summary>
	/// 描画データの追加
	/// </summary>
	/// <param name="renderData">描画データ</param>
	void AddRenderData(const CameraRenderData& renderData);
private://メンバ関数
	/// <summary>
	/// カメラリソースの生成
	/// </summary>
	/// <param name="gpuResource">gpuリソース</param>
	void CreateCameraResource(GpuResource& gpuResource);
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//描画データ
	std::vector<CameraRenderData>renderDatas_;
	//GPUのリソース
	std::vector<GpuResource>gpuResources_;
};

