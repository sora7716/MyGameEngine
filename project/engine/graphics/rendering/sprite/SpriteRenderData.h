#pragma once
#include "RenderData.h"
#include "RenderingData.h"
#include "BlendMode.h"
/// <summary>
/// スプライトの描画データ
/// </summary>
struct SpriteRenderData{
	bool isActive = true;
	std::string imageFileName = "";
	MaterialForSprite material = {};
	TransformationMatrixForSprite transformationMatrix = {};
	BlendMode blendMode = BlendMode::kNone;
};