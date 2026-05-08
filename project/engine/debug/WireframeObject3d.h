#pragma once
#include "PrimitiveData.h"
#include "ResourceData.h"
#include "RenderingData.h"
#include "BlendMode.h"
#include <vector>
#include <string>
#include <wrl.h>
#include <d3d12.h>
//前方宣言
class DirectXBase;
class SRVManager;
class WireframeObject3dCommon;
class Model;
class WorldTransform;
class Camera;

enum class ModelType {
	kSphere,
	kCube
};

/// <summary>
/// ワイヤーフレームモデル
/// </summary>
class WireframeObject3d {
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	WireframeObject3d();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~WireframeObject3d();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="wireframeObject3dCommon">ワイヤーフレームオブジェクトの共通部分</param>
	/// <param name="camera">カメラ</param>
	/// <param name="modelType">モデルタイプ</param>
	/// <param name="instanceCount">Objectを出したい数</param>
	/// <param name="transform3dMode">座標変換のモード</param>
	void Initialize(WireframeObject3dCommon* wireframeObject3dCommon, Camera* camera, ModelType modelType, uint32_t instanceCount = 1, Transform3dMode transform3dMode = Transform3dMode::kNormal);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="objectCount">オブジェクトカウント</param>
	void Draw(int32_t objectCount);

	/// <summary>
	/// モデルのセッター
	/// </summary>
	/// <param name="name">モデルの名前</param>
	void SetModel(const std::string& name);

	/// <summary>
	/// カメラのセッター
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(Camera* camera);

	/// <summary>
	/// スケールのセッター
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <param name="scale">スケール</param>
	void SetScale(uint32_t index, const Vector3& scale);

	/// <summary>
	/// 半径のセッター
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <param name="radius">半径</param>
	void SetRadius(uint32_t index, float radius);

	/// <summary>
	/// 回転のセッター
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <param name="quaternion">クォータニオン</param>
	void SetQuaternion(uint32_t index, const Quaternion& quaternion);

	/// <summary>
	/// 平行移動のセッター
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <param name="translate">平行移動</param>
	void SetTranslate(uint32_t index, const Vector3& translate);

	/// <summary>
	/// トランスフォームのセッター
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <param name="transform">トランスフォーム</param>
	void SetTransformData(uint32_t index, const TransformData& transform);

	/// <summary>
	/// uvスケールのセッター
	/// </summary>
	/// <param name="uvScale">スケール</param>
	void SetUVScale(const Vector2& uvScale);

	/// <summary>
	/// uv回転のセッター
	/// </summary>
	/// <param name="uvRotate">回転</param>
	void SetUVRotate(float uvRotate);

	/// <summary>
	/// uv平行移動のセッター
	/// </summary>
	/// <param name="uvTranslate">平行移動</param>
	void SetUVTranslate(const Vector2& uvTranslate);

	/// <summary>
	/// 色のセッター
	/// </summary>
	/// <param name="color">色</param>
	void SetColor(const Vector4& color);

	/// <summary>
	/// テクスチャの変更
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	void SetTexture(const std::string& filePath);

	/// <summary>
	/// ブレンドモードのセッター
	/// </summary>
	/// <param name="blendMode"></param>
	void SetBlendMode(const BlendMode& blendMode);

	/// <summary>
	/// スケールのゲッター
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>スケール</returns>
	const Vector3& GetScale(uint32_t index)const;

	/// <summary>
	/// 回転のゲッター
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>回転</returns>
	const Quaternion& GetQuaternion(uint32_t index)const;

	/// <summary>
	/// 平行移動のゲッター
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>平行移動</returns>
	const Vector3& GetTranslate(uint32_t index)const;

	/// <summary>
	/// uvスケールのゲッター
	/// </summary>
	/// <returns>uvスケール</returns>
	const Vector2& GetUVScale()const;

	/// <summary>
	/// uv回転のゲッター
	/// </summary>
	/// <returns>uv回転</returns>
	const float GetUVRotate()const;

	/// <summary>
	/// uv平行移動のゲッター
	/// </summary>
	/// <returns>uv平行移動</returns>
	const Vector2& GetUVTranslate()const;

	/// <summary>
	/// 色のゲッター
	/// </summary>
	/// <returns>色</returns>
	const Vector4& GetColor()const;

	/// <summary>
	/// トランスフォームデータのゲッター
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>トランスフォームデータ</returns>
	const TransformData& GetTransformData(uint32_t index)const;

	/// <summary>
	/// モデルのゲッター
	/// </summary>
	/// <returns>モデル</returns>
	Model* GetModel();

	/// <summary>
	/// ワールド座標のゲッター
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>ワールド座標</returns>
	Vector3 GetWorldPos(uint32_t index);

	/// <summary>
	/// スケールのゲッター
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>スケール</returns>
	float GetRadius(uint32_t index)const;

	/// <summary>
	/// 球のゲッター
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>球</returns>
	PrimitiveData::Sphere GetSphere(uint32_t index)const;

	/// <summary>
	/// AABBのゲッター
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>AABB</returns>
	PrimitiveData::AABB GetAABB(uint32_t index)const;

	/// <summary>
	/// OBBのゲッター
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns></returns>
	PrimitiveData::OBB GetOBB(uint32_t index)const;
private://メンバ関数
	/// <summary>
	/// 座標変換行列リソースの生成
	/// </summary>
	void CreateTransformationMatrixResource();

	/// <summary>
	/// 座標変換行列リソースのストラクチャバッファの生成
	/// </summary>
	void CreateStructuredBufferForWvp();

	/// <summary>
	/// 座標の更新
	/// </summary>
	void UpdateTransform();

	/// <summary>
	/// ビルボード行列での更新
	/// </summary>
	void UpdateTransformBillboard();
private://メンバ関数テーブル
	//座標の更新をまとめた
	static void (WireframeObject3d::* UpdateTransformTable[])();
private://メンバ変数
	//ワイヤーフレームオブジェクトの共通部分
	WireframeObject3dCommon* wireframeObject3dCommon_ = nullptr;

	//UV座標
	Transform2dData uvTransform_ = {
		.scale = { 1.0f,1.0f },
		.rotate = 0.0f,
		.translate = {0.0f,0.0f}
	};
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//SRVマネージャー
	SRVManager* srvManager_ = nullptr;
	//3Dモデル
	Model* model_ = nullptr;
	//ワールドビュープロジェクションのリソース
	ComPtr<ID3D12Resource>wvpResource_ = nullptr;
	//ワールドビュープロジェクションのポインタ
	TransformationMatrix* wvpPtr_ = nullptr;
	std::vector<TransformationMatrix> wvpData_ = {};
	//カメラ
	Camera* camera_ = nullptr;
	//ワールド座標
	std::vector<TransformData> transforms_ = {};
	Transform3dMode transform3dMode_ = Transform3dMode::kNormal;
	uint32_t srvIndex_ = 0;
	//親
	const WorldTransform* parent_ = nullptr;
	//ノード
	Node node_ = {};
	//ブレンドモード
	BlendMode blendMode_ = BlendMode::kNone;

	//マテリアル
	Material material_ = {};

	//衝突判定に使用するプリミティブのデータ
	std::vector<PrimitiveData::Sphere> spheres_ = {};//球
	std::vector<PrimitiveData::AABB> aabbs_ = {};//AABB
	std::vector<PrimitiveData::OBB> obbs_ = {};//OBB
};