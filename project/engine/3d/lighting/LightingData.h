#pragma once
#include "Color.h"
#include "Vector4.h"
#include "Vector3.h"

//ライトデータ
namespace lightData{
	//平行光源
	struct DirectionalLight{
		Color color = Color::GetWhite();//ライトの色
		Vector3 direction = {};//ライトの向き
		float intensity = 1.0f;//輝度
		int32_t enable = 0;//平行光源を有効にするか
	};

	//点光源
	struct PointLight{
		Color color = Color::GetWhite();//ライトの色
		Vector3 position = {};//ライトの位置
		float intensity = 1.0f;//輝度
		float distance = 0.0f;//ライトの届く最大距離
		float decay = 0.0f;//減衰率
		int32_t enable = 0;//点光源を有効にするか
	};

	//スポットライト
	struct SpotLight{
		Color color = Color::GetWhite();//ライト色
		Vector3 position = {};//ライトの位置
		float intensity = 1.0f;//輝度
		Vector3 direction = {};//スポットライトの方向
		float distance = 0.0f;//ライトの届く最大距離
		float decay = 0.0f;//減衰率
		float cosAngle = 0.0f;//スポットライトの余弦
		float cosFalloffStart = 0.0f;//
		int32_t enable = 0;//点光源を有効にするか
	};
}