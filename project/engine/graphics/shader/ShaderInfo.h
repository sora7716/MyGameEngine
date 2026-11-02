#pragma once
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <wrl.h>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
//パラメータタイプ
enum class ParameterType :uint32_t{
	kUnknow,
	kFloat,
	kVector2,
	kVector3,
	kColor,
	kInt,
	kUint,
};

//パラメータの情報
struct ParameterInfo{
	ParameterType type;
	uint32_t offset = 0;//パラメータの位置
	uint32_t size = 0;//パラメータのサイズ
};

//CBufferの情報
struct ConstantBufferInfo {
	uint32_t bufferSize;
	uint32_t registerNum;
	std::unordered_map<std::string , ParameterInfo>parameters;
};

//シェーダの情報
struct ShaderInfo{
	Microsoft::WRL::ComPtr<IDxcBlob>shaderBlob = nullptr;
	std::unordered_map<std::string, ConstantBufferInfo>constantBuffers;
};