#pragma once
#include "BlendMode.h"
#include "Matrix4x4.h"
#include "Vector4.h"
#include <vector>

//デバッグ描画で必要な描画データ
struct DebugDrawRenderData{
	BlendMode blendMode = BlendMode::kNone;
	std::vector<Vector4>vertices = {};
	std::vector<uint32_t>indices = {};
	Vector4 material = {};
	Matrix4x4 worldMatrix = {};
};