#pragma once

enum class BlendMode {
	//ブレンドなし
	kNone,
	//通常のαブレンド Src * SrcA + Dest * (1 - SrcA)
	kNormal,
	//加算 Src * SrcA + Desct * 1
	kAdd,
	//減算 Dest * 1 - Src * SrcA
	kSubtract,
	//乗算 Src * 0 + Dest * Src
	kMultily,
	//スクリーン Src * ( 1 - Dest ) + Dest * 1
	kScreen,
	//利用してはいけない(ブレンドモードの数)
	kCountOfBlendMode,
};
