#pragma once
#include "Vector4.h"
#include "Vector3.h"

//平行光源
struct DirectionalLight{
	Vector4 color = Vector4::MakeWhiteColor();//ライトの色
	Vector3 direction = {};//ライトの向き
	float intensity = 1.0f;//輝度
	int32_t isLambert = 0;//lambertにするかどうか
	int32_t isBlinnPhong = 0;//BlinnPhongReflectionを行うかどうか
	int32_t enableDirectionalLighting = 0;//平行光源を有効にするか
};

//点光源
struct PointLight{
	Vector4 color = Vector4::MakeWhiteColor();//ライトの色
	Vector3 position = {};//ライトの位置
	float intensity = 1.0f;//輝度
	float distance = 0.0f;//ライトの届く最大距離
	float decay = 0.0f;//減衰率
	int32_t isBlinnPhong = 0;//BlinnPhongReflectionを行うかどうか
	int32_t enablePointLighting = 0;//点光源を有効にするか
};

//スポットライト
struct SpotLight{
	Vector4 color = Vector4::MakeWhiteColor();//ライト色
	Vector3 position = {};//ライトの位置
	float intensity = 1.0f;//輝度
	Vector3 direction = {};//スポットライトの方向
	float distance = 0.0f;//ライトの届く最大距離
	float decay = 0.0f;//減衰率
	float cosAngle = 0.0f;//スポットライトの余弦
	float cosFalloffStart = 0.0f;//
	int32_t isBlinnPhong = 0;//BlinnPhongReflectionを行うかどうか
	int32_t enableSpotLighting = 0;//点光源を有効にするか
	float padding[2] = {};
};