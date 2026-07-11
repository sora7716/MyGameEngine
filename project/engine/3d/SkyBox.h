#pragma once
#include "RenderData.h"
#include "BlendMode.h"
#include "RenderingData.h"
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
class TextureManager;
class Camera;
class Blend;
class GraphicsPipeline;
class GameObject;

/// <summary>
/// スカイボックス
/// </summary>
class SkyBox {
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
	/// <param name="spriteCommon">スプライトの共通部分</param>
	/// <param name="spriteName">スプライト名</param>
	void Initialize(DirectXBase* directXBase, TextureManager* textureManager, const std::string& imageFileName, Camera* camera);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// ゲームオブジェクトの設定
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	void SetGameObject(GameObject*gameObject);

	/// <summary>
	/// 描画する用のカメラの設定
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetRenderCamera(Camera* camera);
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
	Camera* renderCamera_ = nullptr;
	//テクスチャ番号
	std::string imageFileName_ = "";
	//GameObject
	GameObject* gameObject_ = nullptr;
	//UVトランスフォーム
	Transform2d uvTransform_ = {};
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//ルートシグネイチャ
	ComPtr<ID3D12RootSignature>rootSignature_ = nullptr;
	//グラフィックスパイプライン(PSO)
	std::array<ComPtr<ID3D12PipelineState>, static_cast<int32_t>(BlendMode::kCountOfBlendMode)> graphicsPipelineStates_ = { nullptr };
	//グラフィックスパイプライン
	std::unique_ptr<GraphicsPipeline> makeGraphicsPipeline_ = nullptr;

	//バッファリソース
	ComPtr<ID3D12Resource> directionalLightResource_ = nullptr;//平行光源
	ComPtr<ID3D12Resource> pointLightResource_ = nullptr;//点光源
	//バッファリソース内のデータを指すポインタ
	DirectionalLight* directionalLightPtr_ = nullptr;//平行光源
	PointLight* pointLightPtr_ = nullptr;//点光源

	//テクスチャマネージャー
	TextureManager* textureManager_ = nullptr;

	//ブレンド
	std::unique_ptr<Blend> blend_ = nullptr;

	//バッファリソース
	ComPtr<ID3D12Resource>vertexResource_ = nullptr;//頂点
	ComPtr<ID3D12Resource>indexResource_ = nullptr;//インデックス
	ComPtr<ID3D12Resource>materialResource_ = nullptr;//マテリアル

	//バッファリソース内のデータを指すポインタ
	struct SkyBoxProp {
		Vector4 vertexPos;
		Vector3 texcoord;
	};
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
};
