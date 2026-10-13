#pragma once
#include "BlendMode.h"
#include "Color.h"
#include "Matrix4x4.h"
#include <string>

/// <summary>
/// スカイボックスの描画データ
/// </summary>
struct SkyBoxRenderData{
	bool isActive = true;
	Color material = {};
	Matrix4x4 worldMatrix;
	std::string imageFileName = "";
	BlendMode blendMode = BlendMode::kNone;
};