#include "RenderingData.h"

//初期化
void Transform::Initialize() {
	scale = Vector3::MakeAllOne();
	quaternion = Quaternion::IdentityQuaternion();
	translate = { 0.0f,0.0f,0.0f };
	//デバッグ用
	axis = { 0.0f,1.0f,0.0f };
	angle = 0.0f;
	eulerAngle = { 0.0f,0.0f,0.0f };
	isUsingQuaternion = false;
}

//初期化
void Transform2d::Initialize() {
	this->scale = Vector2::MakeAllOne();
	this->rotate = 0.0f;
	this->translate = { 0.0f,0.0f };
}
