#pragma once
#include "Model.h"
#include "ResourceData.h"
#include "gameObject/GameObjectData.h"
#include "BlendMode.h"
#include "WorldTransform.h"
#include <vector>
#include <string>
#include <wrl.h>
#include <d3d12.h>
//前方宣言
class DirectXBase;
class SRVManager;
class Object3dCommon;
class Camera;

/// <summary>
/// 箱
/// </summary>
class Box {
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Box() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Box();

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
	/// クォータニオンのセッター
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
	/// ゲームオブジェクトのセッター
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <param name="gameObject">ゲームオブジェクト</param>
	void SetGameObject(uint32_t index, const GameObject& gameObject);

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
	/// 親のセッター
	/// </summary>
	/// <param name="parent">親</param>
	void SetParent(const WorldTransform* parent);

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
	/// クォータニオンのゲッター
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>クォータニオン</returns>
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
	/// ゲームオブジェクトのゲッター
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>ゲームオブジェクト</returns>
	const GameObject& GetGameObject(uint32_t index)const;

	/// <summary>
	/// ワールドマトリックスのゲッター
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>ワールドマトリックス</returns>
	Matrix4x4& GetWorldMatrix(uint32_t index);

	/// <summary>
	/// ワールド座標のゲッター
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
	/// 座標の更新
	/// </summary>
	void UpdateTransform();

	/// <summary>
	/// ビルボード行列での更新
	/// </summary>
	void UpdateTransformBillboard();
private://メンバ関数テーブル
	//座標の更新をまとめた
	static void (Box::* UpdateTransformTable[])();
private://メンバ変数
	//3Dオブジェクトの共通部分
	Object3dCommon* object3dCommon_ = nullptr;

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
	//ワールドトランスフォーム
	WorldTransform* worldTransform_ = nullptr;
	//ワールドビュープロジェクションのリソース
	ComPtr<ID3D12Resource>wvpResource_ = nullptr;
	//ワールドビュープロジェクションのポインタ
	TransformationMatrix* wvpPtr_ = nullptr;
	std::vector<TransformationMatrix> wvpData_ = {};
	//カメラ
	Camera* camera_ = nullptr;
	//ワールド座標
	std::vector<GameObject> gameObjects_ = {};
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

	//生成数
	uint32_t aliveCount_ = 1;
};