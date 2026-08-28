#pragma once
#include "MatrixUtility.h"
#include "BlendMode.h"
#include "RenderingData.h"
#include "Object3dRenderData.h"
#include "Component.h"
#include <vector>
#include <string>
#include <memory>

//前方宣言
class Model;
class GameObject;
class MaterialInstance;
class LODController;
class Culling;

/// <summary>
/// 3Dオブジェクト
/// </summary>
class Object3d :public Component{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit Object3d(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Object3d()override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 複製
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <returns>コンポーネント</returns>
	std::unique_ptr<Component>Clone(GameObject* gameObject)const override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// カメラとの距離からLODを更新
	/// </summary>
	/// <param name="distance">カメラとの距離</param>
	void UpdateLOD(float distance);

	/// <summary>
	/// ワールド行列を作成
	/// </summary>
	/// <param name="cameraWorldMatrix">カメラのワールド行列</param>
	/// <returns>ワールド行列</returns>
	Matrix4x4 MakeRenderWorldMatrix(const Matrix4x4& cameraWorldMatrix)const;

	/// <summary>
	/// モデルの設定
	/// </summary>
	/// <param name="model">モデル</param>
	void SetModel(Model* model);

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
	/// <param name="index">マテリアルスロット番号の検索キー</param>
	/// <param name="uvScale">スケール</param>
	void SetUVScale(uint32_t index, const Vector2& uvScale);

	/// <summary>
	/// uv回転の設定
	/// </summary>
	/// <param name="index">マテリアルスロット番号の検索キー</param>
	/// <param name="uvRotate">回転</param>
	void SetUVRotate(uint32_t index, float uvRotate);

	/// <summary>
	/// uv平行移動の設定
	/// </summary>
	/// <param name="index">マテリアルスロット番号の検索キー</param>
	/// <param name="uvTranslate">平行移動</param>
	void SetUVTranslate(uint32_t index, const Vector2& uvTranslate);

	/// <summary>
	/// 色の設定
	/// </summary>
	/// <param name="index">マテリアルスロット番号の検索キー</param>
	/// <param name="color">色</param>
	void SetColor(uint32_t index, const Vector4& color);

	/// <summary>
	/// テクスチャの設定
	/// </summary>
	/// <param name="index">マテリアルスロット番号の検索キー</param>
	/// <param name="imageFileName">画像のファイル名</param>
	void SetTexture(uint32_t index, const std::string& imageFileName);

	/// <summary>
	/// 環境マップの設定
	/// </summary>
	/// <param name="index">マテリアルスロット番号の検索キー</param>
	/// <param name="environmentMapFileName">環境マップのファイル名</param>
	void SetEnvironmentMap(uint32_t index, const std::string& environmentMapFileName);

	/// <summary>
	/// ライティングフラグの設定
	/// </summary>
	/// <param name="index">マテリアルスロット番号の検索キー</param>
	/// <param name="isLighting">ライティングフラグ</param>
	void SetIsLighting(uint32_t index, bool isLighting);

	/// <summary>
	/// 輝度の設定
	/// <param name="index">マテリアルスロット番号の検索キー</param>
	/// </summary>
	/// <param name="shininess">輝度</param>
	void SetShininess(uint32_t index, float shininess);

	/// <summary>
	/// 環境マップの映り込み度を調整
	/// </summary>
	/// <param name="index">マテリアルスロット番号の検索キー</param>
	/// <param name="environmentCoefficient">k環境マップの映り込み度</param>
	void SetEnvironmentCoefficient(uint32_t index, float& environmentCoefficient);

	/// <summary>
	/// UV座標の設定
	/// </summary>マテリアルスロット番号の検索キー
	/// <param name="index">マテリアルスロット番号の検索キー</param>
	/// <param name="uvTransform">UV座標</param>
	void SetUVTransform(uint32_t index, const RectTransform& uvTransform);

	/// <summary>
	/// ブレンドモードの設定
	/// </summary>
	/// <param name="blendMode">ブレンドモード</param>
	void SetBlendMode(BlendMode blendMode);

	/// <summary>
	/// 描画時のトランスフォームモードの設定
	/// </summary>
	/// <param name="transformMode">描画時のトランスフォームモード</param>
	void SetRenderTransformMode(RenderTransformMode transformMode);

	/// <summary>
	/// ワールド行列の取得
	/// </summary>
	/// <returns>ワールド行列</returns>
	const Matrix4x4& GetWorldMatrix()const;

	/// <summary>
	/// ワールド座標の取得
	/// </summary>
	/// <param name="instanceIndex">インスタンスのマテリアルスロット番号の検索キー</param>
	/// <returns>ワールド座標</returns>
	Vector3 GetWorldPos();

	/// <summary>
	/// メッシュのサイズの取得
	/// </summary>
	/// <returns>メッシュのサイズ</returns>
	uint32_t GetMeshDataSize();

	/// <summary>
	/// モデルの取得
	/// </summary>
	/// <returns>モデル</returns>
	Model* GetModel();

	/// <summary>
	/// モデルの取得
	/// </summary>
	/// <returns>モデル</returns>
	const Model* GetModel()const;

	/// <summary>
	/// モデルが設定されているかどうか
	/// </summary>
	/// <returns>モデルが設定されているかどうか</returns>
	bool HasModel()const;

	/// <summary>
	/// ブレンドモードの取得
	/// </summary>
	/// <returns>ブレンドモード</returns>
	BlendMode GetBlendMode()const;

	/// <summary>
	/// 描画時のトランスフォームモードの取得
	/// </summary>
	/// <returns>描画時のトランスフォームモード</returns>
	RenderTransformMode GetRenderTransformMode()const;

	/// <summary>
	/// マテリアルインスタンスの取得
	/// </summary>
	/// <returns>マテリアルインスタンス</returns>
	MaterialInstance* GetMaterialInstance();

	/// <summary>
	/// マテリアルインスタンスの取得
	/// </summary>
	/// <returns>マテリアルインスタンス</returns>
	const MaterialInstance* GetMaterialInstance()const;

	/// <summary>
	/// 現在のLODに対応した描画用モデルを取得
	/// </summary>
	/// <returns>描画用モデル</returns>
	Model* GetRenderModel();

	/// <summary>
	/// 現在のLOD番号を取得
	/// </summary>
	/// <returns>現在のLOD番号</returns>
	uint32_t GetCurrentLOD()const;
private://メンバ関数
	/// <summary>
	/// ワールド行列を作成
	/// </summary>
	void MakeWorldMatrix();

	/// <summary>
	/// マテリアルを個別化する
	/// </summary>
	void EnsureUniqueMaterialInstance();
private://定数
	//インスタンスの最大数
	static const inline uint32_t kMaxInstanceCount_ = 1024;
private://メンバ変数
	//ブレンドモード
	BlendMode blendMode_ = BlendMode::kNone;

	//モデル
	Model* baseModel_ = nullptr;

	//このObject3dが使用するマテリアル
	std::shared_ptr<MaterialInstance>materialInstance_ = nullptr;

	//今現在のLOD番号
	uint32_t currentLOD_ = 0;
	//LODの制御
	std::unique_ptr<LODController>lodController_ = nullptr;

	//オブジェクトの見た目
	RenderTransformMode renderTransformMode_ = RenderTransformMode::kNormal;
	//ワールド行列
	Matrix4x4 worldMatrix_ = {};
	//ノード
	Node node_ = {};
};