#pragma once
#include "RenderData.h"
#include "RenderingData.h"
#include "BlendMode.h"
#include "RasterizerMode.h"
#include <string>
/// <summary>
/// スプライトの描画データ
/// </summary>
struct SpriteRenderData{
	bool isActive = true;
	std::string imageFileName = "";
	MaterialForSprite material = {};
	TransformationMatrixForSprite transformationMatrix = {};
	std::string pixelShaderFilePath = "";
	BlendMode blendMode = BlendMode::kNone;
	rasterizerMode::CullingMode cullingMode = rasterizerMode::CullingMode::kNone;
	rasterizerMode::FillMode fillMode = rasterizerMode::FillMode::kSolid;
};