#pragma once

enum class BlendMode {
	//ブレンドなし
	kNone,
	//通常のαブレンド Src * SrcA + Dest * (1 - SrcA)
	kNormal,
	//加算 Src * SrcA + Dest * 1
	kAdd,
	//減算 Dest * 1 - Src * SrcA
	kSubtract,
	//乗算 Src * 0 + Dest * Src
	kMultily,
	//スクリーン Src * ( 1 - Dest ) + Dest * 1
	kScreen,
	//プリマルチプライドアルファ
	kPremultipliedAlpha,
	//比較(明)
	kLighten,
	//比較(暗)
	kDarken,
	//反転
	kInvert,
	//利用してはいけない(ブレンドモードの数)
	kCountOfBlendMode,
};
