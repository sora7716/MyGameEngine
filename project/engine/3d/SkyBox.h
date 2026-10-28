#pragma once
#include "RenderData.h"
#include "BlendMode.h"
#include "RenderingData.h"
#include "SkyBoxRenderData.h"
#include "Component.h"
#include "Color.h"
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
    /// 複製
    /// </summary>
    /// <param name="gameObject">ゲームオブジェクト</param>
    /// <returns>コンポーネント</returns>
	std::unique_ptr<Component>Clone(GameObject* gameObject)const override;

	/// <summary>
	/// キューブマップの設定
	/// </summary>
	/// <param name="cubeMap">キューブマップ</param>
	void SetCubeMap(const std::string& cubeMap);

	/// <summary>
	/// ピクセルシェーダファイルパスの設定
	/// </summary>
	/// <param name="pixelShaderFilePath">ピクセルシェーダファイルパス</param>
	void SetPixelShaderFilePath(const std::string& pixelShaderFilePath);

	/// <summary>
	/// ブレンドモードの設定
	/// </summary>
	/// <param name="blendMode">ブレンドモード</param>
	void SetBlendMode(BlendMode blendMode);

	/// <summary>
	/// カリングモードの設定
	/// </summary>
	/// <param name="cullingMode">カリングモード</param>
	void SetCullingMode(rasterizerMode::CullingMode cullingMode);

	/// <summary>
	/// フィルモードの設定
	/// </summary>
	/// <param name="fillMode">フィルモード</param>
	void SetFillMode(rasterizerMode::FillMode fillMode);

	/// <summary>
	/// 描画データの取得
	/// </summary>
	/// <returns>描画データ</returns>
	const SkyBoxRenderData& GetRenderData();

	/// <summary>
	/// コンポーネントのタイプ名を取得
	/// </summary>
	/// <returns>コンポーネントのタイプ名</returns>
	std::string GetTypeName()const override;
private://メンバ関数
	/// <summary>
	/// ワールド座標の更新
	/// </summary>
	void UpdateTransform();

	/// <summary>
	/// 描画に必要なデータのセットアップ
	/// </summary>
	void SetupRenderData();
private://メンバ変数
	//ピクセルシェーダファイルパス
	std::string pixelShaderFilePath_ = "SkyBox.PS.hlsl";
	//ブレンドモード
	BlendMode blendMode_ = BlendMode::kNone;
	//カリングモード
	rasterizerMode::CullingMode cullingMode_ = rasterizerMode::CullingMode::kBack;
	//フィルモード
	rasterizerMode::FillMode fillMode_ = rasterizerMode::FillMode::kSolid;

	//画像のファイル名
	std::string imageFileName_ = "";

	//マテリアル
	Color material_ = Color::GetWhite();

	//ワールド行列
	Matrix4x4 worldMatrix_ = {};

	//描画データ
	SkyBoxRenderData renderData_ = {};
};
