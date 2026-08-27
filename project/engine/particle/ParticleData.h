#pragma once
#include "PrimitiveData.h"
#include "Vector4.h"

//パーティクル単体のデータ
struct Particle{
	Transform transform = {};//SRVの情報
	Vector3 velocity = {};//方向
	Vector4 color = Vector4::MakeWhiteColor();//色
	float lifeTime = 0.0f;//生存時間
	float currentTime = 0.0f;//発生してからの
	bool isEnabled = true;//表示するか
};

//発生源
struct Emitter{
	Vector3 translate = {};//エミッターのTransform
	uint32_t count = 0;//発生数
	float frequency = 0.0f;//発生頻度
	float frequencyTime = 0.0f;//頻度用時刻
	float range = 0.0f;//発生範囲
};

//フィールドの加速度
struct AccelerationField{
	Vector3 acceleration = {};//加速度
	primitiveData::AABB area = {};//範囲
};