#pragma once
#include "Component.h"
#include "WireframeRenderData.h"
#include <memory>
#include <vector>

/// <summary>
/// Gizmoの情報を見えるものにする
/// </summary>
class GizmoVisualizer{
public://静的メンバ関数
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GizmoVisualizer();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GizmoVisualizer();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 選択されたゲームオブジェクトを設定
	/// </summary>
	/// <param name="selectedGameObject">選択されたゲームオブジェクトの設定</param>
	void SetSelectedGameObject(GameObject* selectedGameObject);

	/// <summary>
	/// ワイヤーフレームの描画データの配列の取得
	/// </summary>
	/// <returns>描画データ</returns>
	const std::vector<WireframeRenderData>& GetWireframeRenderDatas()const;
private://メンバ関数
	/// <summary>
	/// コライダーごとに適当なワイヤーフレームを決める
	/// </summary>
	void SelectColliderWireframe();
private://メンバ変数
	//選択されたゲームオブジェクト
	GameObject* selectedGameObject_ = nullptr;
	//ワイヤーフレームの描画データの配列
	std::vector<WireframeRenderData>renderDatas_;
};

