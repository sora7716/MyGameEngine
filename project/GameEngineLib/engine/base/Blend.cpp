#include "Blend.h"
#include <cstdint>
#pragma comment(lib,"d3d12.lib")
//ブレンドモードのテーブルの初期化
D3D12_BLEND_DESC(Blend::* Blend::BlendModeTable[])() = {
	&BlendNone,
	&BlendNormal,
	&BlendAdd,
	&BlendSubtract,
	&BlendMultily,
	&BlendScreen
};

//ブレンドデスクをセット
D3D12_BLEND_DESC Blend::SetBlendDesc(BlendMode blendMode) {
	return (this->*BlendModeTable[(int32_t)blendMode])();
}

//ブレンドを開始するときの共通部分
D3D12_BLEND_DESC Blend::BlendModeCommon() {
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	return blendDesc;
}

//ブレンドなし
D3D12_BLEND_DESC Blend::BlendNone() {
	D3D12_BLEND_DESC blendDesc{};
	//ブレンドをしない
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return blendDesc;
}

//ノーマルブレンド(αブレンド)
D3D12_BLEND_DESC Blend::BlendNormal() {
	D3D12_BLEND_DESC blendDesc = BlendModeCommon();
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	return blendDesc;
}

//加算
D3D12_BLEND_DESC Blend::BlendAdd() {
	D3D12_BLEND_DESC blendDesc = BlendModeCommon();
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	return blendDesc;
}

//減算
D3D12_BLEND_DESC Blend::BlendSubtract() {
	D3D12_BLEND_DESC blendDesc = BlendModeCommon();
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	return blendDesc;
}

//乗算
D3D12_BLEND_DESC Blend::BlendMultily() {
	D3D12_BLEND_DESC blendDesc = BlendModeCommon();
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;

	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	return blendDesc;
}

//スクリーン
D3D12_BLEND_DESC Blend::BlendScreen() {
	D3D12_BLEND_DESC blendDesc = BlendModeCommon();
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	return blendDesc;
}
