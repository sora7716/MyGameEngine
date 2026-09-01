#pragma once
#include "d3d12.h"

//ラスタライザモード
namespace rasterizerMode{

	//ポリゴンを“塗るか”“骨組みだけ描くか”を決める設定
	enum class FillMode{
		kSolid = D3D12_FILL_MODE_SOLID,
		kWireframe = D3D12_FILL_MODE_WIREFRAME
	};

	//カリングモード
	enum class CullingMode{
		kBack = D3D12_CULL_MODE_BACK,
		kFront = D3D12_CULL_MODE_FRONT,
		kNone = D3D12_CULL_MODE_NONE
	};
}

