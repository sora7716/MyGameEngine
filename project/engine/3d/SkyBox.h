#pragma once
#include "RenderData.h"
#include "BlendMode.h"
#include "RenderingData.h"
#include "SkyBoxRenderData.h"
#include "Component.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <wrl.h>
#include <array>
#include <string>
#include <memory>

//前方宣言
class DirectXBase;
class Camera;
class GraphicsPipeline;

/// <summary>
/// スカイボックス
/// </summary>
class SkyBox :public Component{
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
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
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="imageFileName">画像のファイル名</param>
	void Initialize(DirectXBase* directXBase, const std::string& imageFileName);

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画に必要なデータのセットアップ
	/// </summary>
	void SetupRenderData();

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
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;

	//ブレンドモード
	BlendMode blendMode_ = BlendMode::kNone;

	//画像のファイル名
	std::string imageFileName_ = "";

	//マテリアル
	Vector4 material_ = {};

	//ワールド行列
	Matrix4x4 worldMatrix_ = {};

	//描画データ
	SkyBoxRenderData renderData_ = {};
};
