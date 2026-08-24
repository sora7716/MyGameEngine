#pragma once
#include "BlendMode.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include <string>

/// <summary>
/// スカイボックスの描画データ
/// </summary>
struct SkyBoxRenderData{
	bool isActive = true;
	Vector4 material = {};
	Matrix4x4 worldMatrix;
	std::string imageFileName = "";
	BlendMode blendMode = BlendMode::kNone;
};