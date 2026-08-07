#pragma once
#include "RenderData.h"
#include "BlendMode.h"
#include "RenderingData.h"
#include "SkyBoxRenderData.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <wrl.h>
#include <array>
#include <string>
#include <memory>

//前方宣言
class DirectXBase;
class Camera;
class GraphicsPipeline;
class GameObject;

/// <summary>
/// スカイボックス
/// </summary>
class SkyBox{
private://構造体など
	struct SkyBoxProp{
		Vector4 vertexPos;
		Vector3 texcoord;
	};
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SkyBox();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SkyBox();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="imageFileName">画像のファイル名</param>
	/// <param name="camera">描画に使用するカメラ</param>
	void Initialize(DirectXBase* directXBase,const std::string& imageFileName, Camera* camera);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// ゲームオブジェクトの設定
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	void SetGameObject(GameObject* gameObject);

	/// <summary>
	/// 描画する用のカメラの設定
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetRenderCamera(Camera* camera);

	/// <summary>
	/// 描画データの取得
	/// </summary>
	/// <returns>描画データ</returns>
	const SkyBoxRenderData& GetSkyBoxRenderData();
private://メンバ関数
	/// <summary>
	/// 頂点データの初期化
	/// </summary>
	void InitializeVertexData();

	/// <summary>
	/// 頂点リソースの生成
	/// </summary>
	void CreateVertexResource();

	/// <summary>
	/// インデックスデータの初期化
	/// </summary>
	void InitializeIndexData();

	/// <summary>
	/// インデックスリソースの生成
	/// </summary>
	void CreateIndexResource();

	/// <summary>
	/// マテリアルデータの初期化
	/// </summary>
	void InitializeMaterialData();

	/// <summary>
	/// マテリアルリソースの生成
	/// </summary>
	void CreateMaterialResource();

	/// <summary>
	/// 座標変換行列リソースの生成
	/// </summary>
	void CreateTransformationMatrixResource();

	/// <summary>
	/// UVの座標変換の更新
	/// </summary>
	void UpdateUVTransform();

	/// <summary>
	/// ワールド座標の更新
	/// </summary>
	void UpdateTransform();
private://定数
	//頂点数
	static inline const uint32_t kVertexCount = 24;
	//インデックス数
	static inline const uint32_t kIndexCount = 36;
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//描画用のカメラ
	Camera* renderCamera_ = nullptr;
	//画像のファイル名
	std::string imageFileName_ = "";
	//GameObject
	GameObject* gameObject_ = nullptr;
	//UVトランスフォーム
	Transform2d uvTransform_ = {};

	//バッファリソース
	ComPtr<ID3D12Resource>vertexResource_ = nullptr;//頂点
	ComPtr<ID3D12Resource>indexResource_ = nullptr;//インデックス
	ComPtr<ID3D12Resource>materialResource_ = nullptr;//マテリアル

	//バッファリソース内のデータを指すポインタ
	std::vector<SkyBoxProp>skyBoxProp;
	std::vector<uint32_t>index_;
	Material* materialData_ = nullptr;//マテリアル

	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};//頂点
	D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};//インデックス

	//ブレンドモード
	BlendMode blendMode_ = BlendMode::kNone;

	//ワールドビュープロジェクションのリソース
	ComPtr<ID3D12Resource>wvpResource_ = nullptr;
	//ワールドビュープロジェクションのデータ
	TransformationMatrix* wvpData_ = nullptr;

	//描画データ
	SkyBoxRenderData skyBoxRenderData_ = {};
};
