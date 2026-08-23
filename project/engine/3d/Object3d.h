#pragma once
#include "MatrixUtility.h"
#include "BlendMode.h"
#include "PrimitiveData.h"
#include "RenderingData.h"
#include "Object3dRenderData.h"
#include "Object3dGpuResource.h"
#include "Component.h"
#include <vector>
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include <array>
#include <memory>

//前方宣言
class Model;
class GameObject;
class MaterialInstance;
class LODBuilder;
class LODController;
class Culling;
class Object3dRenderer;

/// <summary>
/// 3Dオブジェクト
/// </summary>
class Object3d :public Component{
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit Object3d(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Object3d();

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
	/// ワールド行列を作成
	/// </summary>
	/// <param name="cameraWorldMatrix">カメラのワールド行列</param>
	/// <returns>ワールド行列</returns>
	Matrix4x4 MakeRenderWorldMatrix(const Matrix4x4& cameraWorldMatrix)const;

	/// <summary>
	/// モデルの設定
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="keepRates">モデルの保持する倍率</param>
	void SetModel(Model* model, const std::vector<float>& keepRates = { 1.0f,0.75f,0.5f,0.25f });

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
	void SetUVTransform(uint32_t index, const Transform2d& uvTransform);

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
	/// LODのポリゴンの割合の取得
	/// </summary>
	/// <returns>LODのポリゴンの割合</returns>
	const std::vector<float>& GetLODKeepRates()const;

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
private://メンバ関数
	/// <summary>
	/// ワールド行列を作成
	/// </summary>
	/// <returns>ワールド行列</returns>
	void MakeWorldMatrix();

	/// <summary>
	/// マテリアルを個別化する
	/// </summary>
	void EnsureUniqueMaterialInstance();
private://定数
	//インスタンスの最大数
	static const inline uint32_t kMaxInstanceCount_ = 1024;
private://メンバ変数
	//モデル
	Model* baseModel_ = nullptr;
	//今現在のLOD番号
	uint32_t currentLOD_ = 0;
	//ノード
	Node node_ = {};
	//モデルのポリゴン数の割合
	std::vector<float>lodKeepRates_;

	//LODの数
	uint32_t lodCount_ = 1;

	//LODビルダー
	std::unique_ptr<LODBuilder>lodBuilder_ = nullptr;
	//LODの制御
	std::unique_ptr<LODController>lodController_ = nullptr;

	//このObject3dが使用するマテリアル
	std::shared_ptr<MaterialInstance>materialInstance_ = nullptr;

	//オブジェクトの見た目
	RenderTransformMode renderTransformMode_ = RenderTransformMode::kNormal;
	//ワールド行列
	Matrix4x4 worldMatrix_ = {};
	//ブレンドモード
	BlendMode blendMode_ = BlendMode::kNone;
};