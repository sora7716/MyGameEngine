#pragma once
#include "MatrixUtility.h"
#include "BlendMode.h"
#include "WorldTransform.h"
#include "PrimitiveData.h"
#include "RenderingData.h"
#include "RendererData.h"
#include <vector>
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include <array>
#include <memory>
//前方宣言
class DirectXBase;
class TextureManager;
class SRVManager;
class Camera;
class Model;
class GameObject;
class LODBuilder;
class LODController;
class Culling;

//3dオブジェクトのインスタンスデータ
struct Object3dInstance{
	GameObject* gameObject;
	bool isEnabled;
	uint32_t currentLOD;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="targetGameObject">初期化対象のゲームオブジェクト</param>
	void Initialize(GameObject* targetGameObject);
};

/// <summary>
/// 3Dオブジェクト
/// </summary>
class Object3d{
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://静的メンバ関数
	/// <summary>
	/// インスタンスの生成
	/// </summary>
	/// <returns>インスタンス</returns>
	std::unique_ptr<Object3d>Create(DirectXBase* directXBase, SRVManager* srvManager, Camera* renderCamera, uint32_t maxInstanceCount, Transform3dMode transform3dMode);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Object3d();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Object3d();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="srvManager">SRVの管理</param>
	/// <param name="renderCamera">描画で使用するカメラ</param>
	/// <param name="maxInstanceCount">オブジェクトの最大数</param>
	/// <param name="transformMode">トランスフォームモード</param>
	void Initialize(DirectXBase* directXBase, SRVManager* srvManager, Camera* renderCamera, uint32_t maxInstanceCount = 1, Transform3dMode transform3dMode = Transform3dMode::kNormal);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// モデルの設定
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="textureManager">テクスチャの管理</param>
	/// <param name="keepRates">モデルの保持する倍率</param>
	void SetModel(std::unique_ptr<Model> model, TextureManager* textureManager, const std::vector<float>& keepRates = { 1.0f,0.75f,0.5f,0.25f });

	/// <summary>
	/// インスタンスの追加
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <returns></returns>
	uint32_t AddInstance(GameObject* gameObject);

	/// <summary>
	/// ゲームで使用するカメラの設定
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetGameCamera(Camera* camera);

	/// <summary>
	/// 描画に使用するカメラの設定
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetRenderCamera(Camera* camera);

	/// <summary>
	/// LODの切り替え距離の設定
	/// </summary>
	/// <param name="lodDistances">lod切り替え距離</param>
	void SetLODDistances(const std::vector<float>& lodDistances);

	/// <summary>
	/// ヒステリシス幅の設定
	/// </summary>
	/// <param name="hysteresis">ヒステリシス幅</param>
	void SetHysteresis(float hysteresis);

	/// <summary>
	/// uvスケールの設定
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <param name="uvScale">スケール</param>
	void SetUVScale(uint32_t index, const Vector2& uvScale);

	/// <summary>
	/// uv回転の設定
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <param name="uvRotate">回転</param>
	void SetUVRotate(uint32_t index, float uvRotate);

	/// <summary>
	/// uv平行移動の設定
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <param name="uvTranslate">平行移動</param>
	void SetUVTranslate(uint32_t index, const Vector2& uvTranslate);

	/// <summary>
	/// 色の設定
	/// </summary>
	/// <param name="materialIndex">マテリアルの検索キー</param>
	/// <param name="color">色</param>
	void SetColor(uint32_t materialIndex, const Vector4& color);

	/// <summary>
	/// 親の設定
	/// </summary>
	/// <param name="parent">親</param>
	void SetParent(const WorldTransform* parent);

	/// <summary>
	/// テクスチャの設定
	/// </summary>
	/// <param name="meshIndex">メッシュの検索キー</param>
	/// <param name="imageFileName">画像のファイル名</param>
	void SetTexture(uint32_t meshIndex, const std::string& imageFileName);

	/// <summary>
	/// 環境マップの設定
	/// </summary>
	/// <param name="meshIndex">メッシュの検索キー</param>
	/// <param name="environmentMapFileName">環境マップのファイル名</param>
	void SetEnvironmentMap(uint32_t meshIndex, const std::string& environmentMapFileName);

	/// <summary>
	/// ライティングフラグの設定
	/// </summary>
	/// <param name="meshIndex">メッシュの検索キー</param>
	/// <param name="isLighting">ライティングフラグ</param>
	void SetIsLighting(uint32_t meshIndex, bool isLighting);

	/// <summary>
	/// 輝度の設定
	/// <param name="meshIndex">メッシュの検索キー</param>
	/// </summary>
	/// <param name="shininess">輝度</param>
	void SetShininess(uint32_t meshIndex, float shininess);

	/// <summary>
	/// 環境マップの映り込み度を調整
	/// </summary>
	/// <param name="meshIndex">メッシュの検索キー</param>
	/// <param name="environmentCoefficient">k環境マップの映り込み度</param>
	void SetEnvironmentCoefficient(uint32_t meshIndex, float& environmentCoefficient);

	/// <summary>
	/// UV座標の設定
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <param name="uvTransform">UV座標</param>
	void SetUVTransform(uint32_t index, const Transform2d& uvTransform);

	/// <summary>
	/// ブレンドモードの設定
	/// </summary>
	/// <param name="blendMode"></param>
	void SetBlendMode(const BlendMode& blendMode);

	/// <summary>
	/// uvスケールの取得
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>uvスケール</returns>
	const Vector2& GetUVScale(uint32_t index)const;

	/// <summary>
	/// uv回転の取得
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>uv回転</returns>
	const float GetUVRotate(uint32_t index)const;

	/// <summary>
	/// uv平行移動の取得
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>uv平行移動</returns>
	const Vector2& GetUVTranslate(uint32_t index)const;

	/// <summary>
	/// UV座標の取得
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>UV座標</returns>
	const Transform2d& GetUVTransform(uint32_t index)const;

	/// <summary>
	/// 色の取得
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>色</returns>
	const Vector4& GetColor(uint32_t index)const;

	/// <summary>
	/// ワールドマトリックスの取得
	/// </summary>
	/// <param name="instanceIndex">インスタンスの検索キー</param>
	/// <returns>ワールドマトリックス</returns>
	Matrix4x4& GetWorldMatrix(uint32_t instanceIndex);

	/// <summary>
	/// ワールド座標の取得
	/// </summary>
	/// <param name="instanceIndex">インスタンスの検索キー</param>
	/// <returns>ワールド座標</returns>
	Vector3 GetWorldPos(uint32_t instanceIndex);

	/// <summary>
	/// メッシュのサイズの取得
	/// </summary>
	/// <returns>メッシュのサイズ</returns>
	uint32_t GetMeshDataSize();

	/// <summary>
	/// 描画データの取得
	/// </summary>
	/// <returns>描画データ</returns>
	const Object3dRenderData& GetRenderData();
private://メンバ関数
	/// <summary>
	/// LOD関係のセットアップ
	/// </summary>
	void SetupLOD();

	/// <summary>
	/// 座標変換行列リソースの生成
	/// </summary>
	void CreateTransformationMatrixResource();

	/// <summary>
	/// 座標変換行列リソースのストラクチャバッファの生成
	/// </summary>
	void CreateStructuredBufferForWvp();

	/// <summary>
	/// ワールド行列を作成
	/// </summary>
	/// <param name="instanceIndex">インスタンスの検索キー</param>
	/// <returns>ワールド行列</returns>
	void MakeWorldMatrix(uint32_t instanceIndex);

	/// <summary>
	/// ビルボード行列の作成
	/// </summary>
	/// <param name="instanceIndex">インスタンスの検索キー</param>
	/// <returns>ビルボード行列</returns>
	void MakeBillboardWorldMatrix(uint32_t instanceIndex);

	/// <summary>
	/// 座標の更新
	/// </summary>
	/// <param name="lodIndex">LODの検索キー</param>
	/// <param name="drawIndex">描画の検索キー</param>
	/// <param name="worldMatrix">ワールド行列</param>
	void UpdateWorldTransform(uint32_t lodIndex, uint32_t drawIndex, const Matrix4x4& worldMatrix);
private://メンバ関数テーブル
	//座標の更新をまとめた
	static void (Object3d::* UpdateWorldMatrixTable[])(uint32_t index);
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//SRVマネージャー
	SRVManager* srvManager_ = nullptr;
	//モデル
	std::unique_ptr<Model> baseModel_ = nullptr;

	//LODの数
	uint32_t lodCount_ = 1;
	std::vector<uint32_t>lodIndices_;
	//LODビルダー
	std::unique_ptr<LODBuilder>lodBuilder_ = nullptr;
	//LODの制御
	std::unique_ptr<LODController>lodController_ = nullptr;
	//LODWvpデータ
	std::vector<std::vector<TransformationMatrix>>lodWvpData_;
	//ワールドビュープロジェクションのリソース
	std::vector<ComPtr<ID3D12Resource>> lodWvpResources_;
	//ワールドビュープロジェクションのポインタ
	std::vector<TransformationMatrix*>lodWvpPtrs_;
	std::vector<uint32_t>lodSrvIndices_;
	std::vector<uint32_t>lodDrawCounts_;
	//UV座標
	std::vector<std::vector<Transform2d>> lodUvTransforms_;

	//描画用のカメラ
	Camera* renderCamera_ = nullptr;
	//ゲームで使用するカメラ
	Camera* gameCamera_ = nullptr;

	//インスタンスデータ
	std::vector<Object3dInstance> instanceData_ = {};
	//インスタンスの最大数
	uint32_t maxInstanceCount_ = 0;
	//オブジェクトの見た目
	Transform3dMode transform3dMode_ = Transform3dMode::kNormal;
	//ワールド行列
	std::vector<Matrix4x4> worldMatrixes_;
	//親
	const WorldTransform* parent_ = nullptr;
	//ノード
	Node node_ = {};
	//ブレンドモード
	BlendMode blendMode_ = BlendMode::kNone;

	//マテリアル
	Material material_ = {};

	//カリング
	std::unique_ptr<Culling>culling_ = nullptr;

	//レンダーラークラスに渡す情報
	Object3dRenderData rendererData_;
};