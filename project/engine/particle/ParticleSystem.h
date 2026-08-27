#pragma once
#include "RenderData.h"
#include "BlendMode.h"
#include "ParticleRenderData.h"
#include "Component.h"
#include <memory>

//前方宣言
class Model;
class MaterialInstance;
class ParticleEmitter;
class ParticleRenderer;

/// <summary>
/// パーティクルシステム
/// </summary>
class ParticleSystem :public Component{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit ParticleSystem(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ParticleSystem();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 複製
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <returns>コンポーネント</returns>
	std::unique_ptr<Component>Clone(GameObject* gameObject)const override;

	/// <summary>
	/// ブレンドモードの設定
	/// </summary>
	/// <param name="blendMode">ブレンドモード</param>
	void SetBlendMode(BlendMode blendMode);

	/// <summary>
	/// パーティクルの数の設定
	/// </summary>
	/// <param name="cont">パーティクルの数</param>
	void SetParticleCount(uint32_t cont);

	/// <summary>
	/// 発生範囲の設定
	/// </summary>
	/// <param name="range">範囲</param>
	void SetEmitRange(float range);

	/// <summary>
	/// 加速度が起こるフィールドの設定
	/// </summary>
	/// <param name="field">フィールド</param>
	void SetAccelerationField(const AccelerationField& field);

	/// <summary>
	/// パーティクルの発生感覚[秒]の設定
	/// </summary>
	/// <param name="frequency">発生感覚</param>
	void SetFrequency(float frequency);

	/// <summary>
	/// モデルの設定
	/// </summary>
	/// <param name="model">モデル</param>
	void SetModel(Model* model);

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
	/// UV座標の設定
	/// </summary>マテリアルスロット番号の検索キー
	/// <param name="index">マテリアルスロット番号の検索キー</param>
	/// <param name="uvTransform">UV座標</param>
	void SetUVTransform(uint32_t index, const RectTransform& uvTransform);

	/// <summary>
	/// モデルの取得
	/// </summary>
	/// <returns>モデル</returns>
	Model* GetModel();

	/// <summary>
	/// 描画データの取得
	/// </summary>
	/// <returns>描画データ</returns>
	const ParticleRenderData& GetRenderData();

	/// <summary>
	/// モデルを所有しているか
	/// </summary>
	/// <returns></returns>
	bool HasModel()const;
private://メンバ関数
	/// <summary>
    /// マテリアルを個別化する
    /// </summary>
	void EnsureUniqueMaterialInstance();

	/// <summary>
	/// 描画に必要なデータのセットアップ
	/// </summary>
	void SetupRenderData();
private://メンバ変数
	//ブレンドモード
	BlendMode blendMode_ = BlendMode::kAdd;
	//パーティクルの発生源
	std::unique_ptr<ParticleEmitter>emitter_ = nullptr;
	//モデル
	Model* model_ = nullptr;
	//ノード
	Node node_ = {};
	//このObject3dが使用するマテリアル
	std::shared_ptr<MaterialInstance>materialInstance_ = nullptr;
	//描画データ
	ParticleRenderData renderData_ = {};
};
