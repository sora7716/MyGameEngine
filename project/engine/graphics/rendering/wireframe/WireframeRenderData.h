#pragma once
#include "BlendMode.h"
#include "Matrix4x4.h"
#include "Vector4.h"
#include "Color.h"
#include "RasterizerMode.h"
#include <string>
#include <vector>

//デバッグ描画で必要な描画データ
struct WireframeRenderData{
	std::vector<Vector4>vertices = {};
	std::vector<uint32_t>indices = {};
	Color material = Color::GetWhite();
	Matrix4x4 worldMatrix = {};
	std::string pixelShaderFilePath = "";
	BlendMode blendMode = BlendMode::kNone;
	rasterizerMode::CullingMode cullingMode = rasterizerMode::CullingMode::kNone;
	rasterizerMode::FillMode fillMode = rasterizerMode::FillMode::kSolid;
};