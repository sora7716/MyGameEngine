#pragma once
#include "ResourceData.h"
#include "GameObjectData.h"
#include "BlendMode.h"
#include "WorldTransform.h"
#include "PrimitiveData.h"
#include "RenderingData.h"
#include <vector>
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include <array>
//前方宣言
class DirectXBase;
class SRVManager;
class Object3dCommon;
class Camera;
class Model;

/// <summary>
/// 3Dオブジェクト
/// </summary>
class Object3d {
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Object3d() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Object3d();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="object3dCommon">3dオブジェクトの共通部分</param>
	/// <param name="camera">カメラ</param>
	/// <param name="instanceCount">オブジェクトの表示したい数</param>
	/// <param name="transformMode">トランスフォームモード</param>
	void Initialize(Object3dCommon* object3dCommon, Camera* camera, uint32_t instanceCount = 1, Transform3dMode transform3dMode = Transform3dMode::kNormal);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// モデルの設定
	/// </summary>
	/// <param name="name">モデルの名前</param>
	void SetModel(const std::string& name);

	/// <summary>
	/// LODモデルの設定
	/// </summary>
	/// <param name="lodIndex">インデックス</param>
	/// <param name="modelName">モデル名</param>
	void SetLODModel(uint32_t lodIndex, const std::string& modelName);

	/// <summary>
	/// カメラの設定
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(Camera* camera);

	/// <summary>
	/// スケールの設定
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <param name="scale">スケール</param>
	void SetScale(uint32_t index, const Vector3& scale);

	/// <summary>
	/// クォータニオンの設定
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <param name="quaternion">クォータニオン</param>
	void SetQuaternion(uint32_t index, const Quaternion& quaternion);

	/// <summary>
	/// 平行移動の設定
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <param name="translate">平行移動</param>
	void SetTranslate(uint32_t index, const Vector3& translate);

	/// <summary>
	/// ゲームオブジェクトの設定
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <param name="gameObject">ゲームオブジェクト</param>
	void SetGameObject(uint32_t index, const GameObject& gameObject);

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
	/// テクスチャの変更
	/// </summary>
	/// <param name="materialIndex">マテリアルの検索キー</param>
	/// <param name="filePath">ファイルパス</param>
	void SetTexture(uint32_t materialIndex, const std::string& filePath);

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
	/// 視錐台の設定
	/// </summary>
	/// <param name="frustum">視錐台</param>
	void SetFrustum(PrimitiveData::Frustum* frustum);

	/// <summary>
	/// スケールの取得
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>スケール</returns>
	const Vector3& GetScale(uint32_t index)const;

	/// <summary>
	/// クォータニオンの取得
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>クォータニオン</returns>
	const Quaternion& GetQuaternion(uint32_t index)const;

	/// <summary>
	/// 平行移動の取得
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>平行移動</returns>
	const Vector3& GetTranslate(uint32_t index)const;

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
	/// ゲームオブジェクトの取得
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>ゲームオブジェクト</returns>
	const GameObject& GetGameObject(uint32_t index)const;

	/// <summary>
	/// モデルの取得
	/// </summary>
	/// <returns>モデル</returns>
	Model* GetModel();

	/// <summary>
	/// ワールドマトリックスの取得
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>ワールドマトリックス</returns>
	Matrix4x4& GetWorldMatrix(uint32_t index);

	/// <summary>
	/// ワールド座標の取得
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>ワールド座標</returns>
	Vector3 GetWorldPos(uint32_t index);
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
	/// ワールド行列を作成
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>ワールド行列</returns>
	void MakeWorldMatrix(uint32_t index);

	/// <summary>
	/// ビルボード行列の作成
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>ビルボード行列</returns>
	void MakeBillboardWorldMatrix(uint32_t index);

	/// <summary>
	/// 座標の更新
	/// </summary>
	/// <param name="lodIndex">LODの検索キー</param>
	/// <param name="drawIndex">描画の検索キー</param>
	/// <param name="worldMatrix">ワールド行列</param>
	void UpdateWorldTransform(uint32_t lodIndex,uint32_t drawIndex,const Matrix4x4& worldMatrix);

	/// <summary>
	/// オブジェクトの表示状態の更新
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <param name="worldMatrix">ワールド行列</param>
	void UpdateVisibility(uint32_t index,const Matrix4x4& worldMatrix);

	/// <summary>
	/// 距離によってLODモデルの添え字を取得
	/// </summary>
	/// <param name="distance">距離</param>
	/// <returns>LODモデルの添え字</returns>
	uint32_t SelectLOD(float distance)const;
private://メンバ関数テーブル
	//座標の更新をまとめた
	static void (Object3d::* UpdateWorldMatrixTable[])(uint32_t index);
	//定数
private:
	//LODの数
	static inline const uint32_t kLODCount = 3;
private://メンバ変数
	//3Dオブジェクトの共通部分
	Object3dCommon* object3dCommon_ = nullptr;
	//UV座標
	std::array<std::vector<Transform2d>,kLODCount> lodUvTransforms_;
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//SRVマネージャー
	SRVManager* srvManager_ = nullptr;
	//モデル
	Model* model_ = nullptr;
	
	//LOD用のモデル
	std::vector <Model*>lodModels_;
	//LODの距離
	std::vector<float>lodDistances_;

	std::array<std::vector<TransformationMatrix>, kLODCount>lodWvpData_;
	//ワールドビュープロジェクションのリソース
	std::array<ComPtr<ID3D12Resource>, kLODCount> lodWvpResources_;
	//ワールドビュープロジェクションのポインタ
	std::array<TransformationMatrix*, kLODCount>lodWvpPtrs_;
	std::array<uint32_t, kLODCount>lodSrvIndices_;
	std::array<uint32_t, kLODCount>lodDrawCount_;
	//カメラ
	Camera* camera_ = nullptr;
	//ワールド座標
	std::vector<GameObject> gameObjects_ = {};
	Transform3dMode transform3dMode_ = Transform3dMode::kNormal;
	Matrix4x4 worldMatrix_ = Matrix4x4::Identity4x4();
	//親
	const WorldTransform* parent_ = nullptr;
	//ノード
	Node node_ = {};
	//ブレンドモード
	BlendMode blendMode_ = BlendMode::kNone;

	//マテリアル
	Material material_ = {};

	//視錐台
	PrimitiveData::Frustum* frustum_ = nullptr;
};

