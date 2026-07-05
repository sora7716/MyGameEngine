#pragma once
#include "Vector3.h"
#include <cstdint>
#include <string>

//プリミティブメッシュのタイプ
enum class PrimitiveMeshType : uint32_t {
	kCube,
	kSphere,
	kPlane,
	kNone
};

//プリミティブメッシュを作成するときに使用する
struct PrimitiveMeshCreateDesc {
	PrimitiveMeshType meshType = PrimitiveMeshType::kNone;
	Vector3 size = Vector3::MakeAllOne();
	uint32_t sphereSubdivision = 16;
	std::string nodeName = "";
};
