#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"

//カメラの描画データ
struct CameraRenderData{
	Vector3 worldPosition = {};
	Matrix4x4 viewProjection = Matrix4x4::Identity4x4();
};

//カメラモード
enum class CameraMode{
	kMain,
	kDebug
};