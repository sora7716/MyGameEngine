#pragma once
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <wrl.h>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

//パラメータの情報
struct ParameterInfo{
	uint32_t offset = 0;//パラメータの位置
	uint32_t size = 0;//パラメータのサイズ
};

//シェーダの情報
struct ShaderInfo{
	Microsoft::WRL::ComPtr<IDxcBlob>shaderBlob = nullptr;
	std::unordered_map<std::string, ParameterInfo>parameters;
};

//パラメータタイプ
enum class ParameterType{
	kFloat,
	kVector2,
	kVector3,
	kColor,
	kInt,
	kBool
};