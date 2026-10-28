#pragma once
#include <vector>
#include <string>

//ラスタライザモード
namespace rasterizerMode{

	//ポリゴンを“塗るか”“骨組みだけ描くか”を決める設定
	enum class FillMode{
		kSolid,
		kWireframe
	};

	//カリングモード
	enum class CullingMode{
		kBack,
		kFront,
		kNone
	};
}

//フィルモード一覧
inline const std::vector<std::string> kFillOptions = { "Solid","Wireframe" };
//カリングモード一覧
inline const std::vector<std::string> kCullingOptions = { "Back","Front","None" };

