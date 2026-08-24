#pragma once
#include "RenderData.h"
#include "BlendMode.h"
#include "RenderingData.h"
#include "SkyBoxRenderData.h"
#include "Component.h"
#include <array>
#include <string>
#include <memory>

/// <summary>
/// スカイボックス
/// </summary>
class SkyBox :public Component{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit SkyBox(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SkyBox()override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画に必要なデータのセットアップ
	/// </summary>
	void SetupRenderData();

	/// <summary>
	/// キューブマップの設定
	/// </summary>
	/// <param name="cubeMap">キューブマップ</param>
	void SetCubeMap(const std::string& cubeMap);

	/// <summary>
	/// ブレンドモードの設定
	/// </summary>
	/// <param name="blendMode">ブレンドモード</param>
	void SetBlendMode(BlendMode blendMode);

	/// <summary>
	/// 複製
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <returns>コンポーネント</returns>
	std::unique_ptr<Component>Clone(GameObject* gameObject)const override;

	/// <summary>
	/// 描画データの取得
	/// </summary>
	/// <returns>描画データ</returns>
	const SkyBoxRenderData& GetRenderData();
private://メンバ関数
	/// <summary>
	/// ワールド座標の更新
	/// </summary>
	void UpdateTransform();
private://メンバ変数
	//ブレンドモード
	BlendMode blendMode_ = BlendMode::kNone;

	//画像のファイル名
	std::string imageFileName_ = "";

	//マテリアル
	Vector4 material_ = Vector4::MakeWhiteColor();

	//ワールド行列
	Matrix4x4 worldMatrix_ = {};

	//描画データ
	SkyBoxRenderData renderData_ = {};
};
