#pragma once
#include "BlendMode.h"
#include "Color.h"
#include "Matrix4x4.h"
#include "RasterizerMode.h"
#include <string>

/// <summary>
/// スカイボックスの描画データ
/// </summary>
struct SkyBoxRenderData{
	bool isActive = true;
	Color material = {};
	Matrix4x4 worldMatrix;
	std::string imageFileName = "";
	std::string pixelShaderFilePath = "";
	BlendMode blendMode = BlendMode::kNone;
	rasterizerMode::CullingMode cullingMode = rasterizerMode::CullingMode::kNone;
	rasterizerMode::FillMode fillMode = rasterizerMode::FillMode::kSolid;
};