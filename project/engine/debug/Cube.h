#pragma once
#include "BaseShape.h"
class Cube:public BaseShape {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Cube();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Cube();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="camera">カメラ</param>
	void Initialize(DirectXBase* directXBase, Camera* camera)override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw()override;
private://メンバ変数
	/// <summary>
	/// 頂点データの設定
	/// </summary>
	void SettingVertexData()override;

	/// <summary>
	/// インデックスの設定
	/// </summary>
	void SettingIndexData()override;
private://メンバ変数
	//OBB
	OBB obb_ = {};
};

