#pragma once
#include <d3d12.h>
#include "BlendMode.h"

/// <summary>
/// ブレンド
/// </summary>
class Blend{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Blend() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Blend() = default;

	/// <summary>
	/// ブレンドデスクをセット
	/// </summary>
	/// <param name="blendMode">ブレンドモード</param>
	/// <returns>ブレンドデスク</returns>
	D3D12_BLEND_DESC SetBlendDesc(BlendMode blendMode);

private://メンバ関数
	/// <summary>
	/// ブレンドを開始するときの共通部分
	/// </summary>
	/// <returns></returns>
	D3D12_BLEND_DESC BlendModeCommon();

	/// <summary>
	/// ブレンドなし
	/// </summary>
	/// <returns></returns>
	D3D12_BLEND_DESC BlendNone();

	/// <summary>
	/// ノーマルブレンド(αブレンド)
	/// </summary>
	/// <returns></returns>
	D3D12_BLEND_DESC BlendNormal();

	/// <summary>
	/// 加算
	/// </summary>
	/// <returns></returns>
	D3D12_BLEND_DESC BlendAdd();

	/// <summary>
	/// 減算
	/// </summary>
	/// <returns></returns>
	D3D12_BLEND_DESC BlendSubtract();

	/// <summary>
	/// 乗算
	/// </summary>
	/// <returns></returns>
	D3D12_BLEND_DESC BlendMultily();

	/// <summary>
	/// スクリーン
	/// </summary>
	/// <returns></returns>
	D3D12_BLEND_DESC BlendScreen();
private://メンバ関数ポインタ
	//ブレンドモードのテーブル
	static D3D12_BLEND_DESC(Blend::* BlendModeTable[])();
};

