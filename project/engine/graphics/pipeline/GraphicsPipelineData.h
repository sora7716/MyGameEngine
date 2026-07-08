#pragma once
//ポリゴンを“塗るか”“骨組みだけ描くか”を決める設定
enum class FillMode {
	kSolid = D3D12_FILL_MODE_SOLID,
	kWireframe = D3D12_FILL_MODE_WIREFRAME
};

