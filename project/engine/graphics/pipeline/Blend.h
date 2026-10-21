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
	Blend();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Blend();

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
	D3D12_BLEND_DESC BlendMultiply();

	/// <summary>
	/// スクリーン
	/// </summary>
	/// <returns></returns>
	D3D12_BLEND_DESC BlendScreen();

	/// <summary>
	/// プリマルチプライドアルファ
	/// </summary>
	/// <returns></returns>
	D3D12_BLEND_DESC PremultipliedAlpha();

	/// <summary>
	/// 比較(明)
	/// </summary>
	/// <returns></returns>
	D3D12_BLEND_DESC Lighten();

	/// <summary>
	/// 比較(暗)
	/// </summary>
	/// <returns></returns>
	D3D12_BLEND_DESC Darken();

	/// <summary>
	/// 反転
	/// </summary>
	/// <returns></returns>
	D3D12_BLEND_DESC Invert();
private://メンバ関数ポインタ
	//ブレンドモードのテーブル
	static D3D12_BLEND_DESC(Blend::* BlendModeTable[])();
};

