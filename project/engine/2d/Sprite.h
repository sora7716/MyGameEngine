#pragma once
#include "SpriteRenderData.h"
#include "RenderData.h"
#include "BlendMode.h"
#include "Component.h"
#include <string>

/// <summary>
/// スプライト
/// </summary>
class Sprite :public Component{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit Sprite(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Sprite()override;

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
	/// テクスチャの変更
	/// </summary>
	/// <param name="spriteName">スプライト名</param>
	void ChangeTexture(const std::string& spriteName);

	/// <summary>
	/// 色のセッター
	/// </summary>
	/// <param name="color">色</param>
	void SetColor(const Vector4& color);

	/// <summary>
	/// ブレンドモードのセッター
	/// </summary>
	void SetBlendMode(BlendMode blendMode);

	/// <summary>
	/// UVスケールの設定
	/// </summary>
	/// <param name="scale">スケール</param>
	void SetUVScale(const Vector2& scale);

	/// <summary>
	/// UV回転の設定
	/// </summary>
	/// <param name="rotate">回転</param>
	void SetUVRotate(float rotate);

	/// <summary>
	/// UV平行移動の設定
	/// </summary>
	/// <param name="translate">平行移動</param>
	void SetUVTranslate(const Vector2& translate);

	/// <summary>
	/// UVのトランスフォームの設定
	/// </summary>
	/// <param name="rectTransform">トランスフォーム</param>
	void SetUVRectTransform(const RectTransform& rectTransform);

	/// <summary>
	/// UVスケールの取得
	/// </summary>
	/// <returns>スケール</returns>
	const Vector2& GetUVScale();

	/// <summary>
	/// UV回転の取得
	/// </summary>
	/// <returns>回転</returns>
	float GetUVRotate();

	/// <summary>
	/// UV平行移動の取得
	/// </summary>
	/// <returns>平行移動</returns>
	const Vector2& GetUVTranslate();

	/// <summary>
	/// UVのトランスフォームの取得
	/// </summary>
	/// <returns></returns>
	const RectTransform& GetUVRectTransform();

	/// <summary>
	/// 描画データの取得
	/// </summary>
	/// <returns>描画データ</returns>
	const SpriteRenderData& GetRenderData();
private://メンバ関数
	/// <summary>
	/// ワールド座標の更新
	/// </summary>
	void UpdateTransform();

	/// <summary>
    /// UVの座標変換の更新
    /// </summary>
	void UpdateUVTransform();

	/// <summary>
	/// 描画に必要なデータのセットアップ
	/// </summary>
	void SetupRenderData();
private://メンバ変数
	//テクスチャ番号
	std::string imageFileName_ = {};

	//UVTransform
	RectTransform uvTransform_ = {};

	//ブレンドモード
	BlendMode blendMode_ = BlendMode::kNone;

	//マテリアル
	MaterialForSprite material_ = {};

	//トランスフォーメーション行列
	TransformationMatrixForSprite transformationMatrix_ = {};

	//描画データ
	SpriteRenderData renderData_ = {};
};
