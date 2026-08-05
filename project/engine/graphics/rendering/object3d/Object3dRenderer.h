#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "Object3dRendererData.h"
#include "Object3dGpuResource.h"
#include <memory>

//前方宣言
class DirectXBase;
class SRVManager;
class TextureManager;

/// <summary>
/// Object3dの描画を担当
/// </summary>
class Object3dRenderer{
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://静的メンバ関数
	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="srvManager">SRVの管理</param>
	/// <param name="textureManager">テクスチャの管理</param>
	/// <param name="maxInstance">インスタンスの最大値</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<Object3dRenderer>Create(DirectXBase* directXBase, SRVManager* srvManager, TextureManager* textureManager, uint32_t maxInstance = 1024);
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
	/// <param name="textureManager">テクスチャの管理</param>
	/// <param name="maxInstance">インスタンスの最大値</param>
	void Initialize(DirectXBase* directXBase, SRVManager* srvManager, TextureManager* textureManager, uint32_t maxInstance);

	/// <summary>
	/// オブジェクトを登録
	/// </summary>
	/// <param name="lodCount">Lodの数</param>
	/// <param name="maxInstance">Object3dが表示される最大数</param>
	Object3dRenderHandle RegisterObject(uint32_t lodCount, uint32_t maxInstance);

	/// <summary>
	///リセット
	/// </summary>
	void Reset();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="instanceIndex">インスタンス検索キー</param>
	void Draw(uint32_t instanceIndex);

	/// <summary>
	/// 描画データの追加
	/// </summary>
	/// <param name="renderData">描画データ</param>
	void AddRenderData(const Object3dRenderData& renderData);

	/// <summary>
	/// ブレンドモードの取得
	/// </summary>
	/// <param name="instanceIndex">インスタンス検索キー</param>
	/// <returns>ブレンドモード</returns>
	BlendMode GetBlendMode(uint32_t instanceIndex);

	/// <summary>
	/// 描画データの配列のサイズの取得
	/// </summary>
	/// <returns>描画データの配列のサイズ</returns>
	uint32_t GetRenderDataSize();
private://メンバ関数
	/// <summary>
	/// 座標変換行列リソースの生成
	/// </summary>
	/// <param name="lodGpuResource">LodのGpuに送るデータ</param>
	void CreateTransformationMatrixResource(LODGpuResource& lodGpuResource);

	/// <summary>
	/// 座標変換行列リソースのストラクチャバッファの生成
	/// </summary>
	/// <param name="lodGpuResource">LodのGpuに送るデータ</param>
	void CreateStructuredBufferForWvp(LODGpuResource& lodGpuResource);
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//SRVの管理
	SRVManager* srvManager_ = nullptr;
	//テクスチャの管理
	TextureManager* textureManager_ = nullptr;
	//インスタンスの最大値
	uint32_t maxInstanceCount_ = 0;
	//マイフレーム消す描画データ
	std::vector<Object3dRenderData>renderDatas_;
	//Object3dが生存している間保持する
	std::vector<Object3dGpuResource>objectResources_;
};

