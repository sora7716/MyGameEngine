#pragma once
#include "BaseShape.h"
class Line:public BaseShape{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Line();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Line();

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
	/// 描画
	/// </summary>
	void Draw()override;

	/// <summary>
	/// 頂点データの設定
	/// </summary>
	void SettingVertexData()override;

	/// <summary>
	/// インデックスの設定
	/// </summary>
	void SettingIndexDate()override;
private://メンバ変数
	//線分
	//Segment segment_ = {};
};

