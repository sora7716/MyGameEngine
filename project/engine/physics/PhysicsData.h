#pragma once
#include "Vector3.h"
//ばね
//anchor: アンカー。固定された端の位置
//naturalLength: 自然長
//stiffness: 剛性。ばね定数k
//dampingCoefficient: 減衰係数。ばねの振動を減衰させるための係数
struct Spring {
	Vector3 anchor;
	float naturalLength;
	float stiffness;
	float dampingCoefficient;
};

//振り子
//anchor: アンカー。固定された端の位置
//length: 紐の長さ
//angle: 現在の角度
//angularVelocity: 角速度ω
//angularAcceleration: 角加速度
struct Pendulum {
	Vector3 anchor;
	float length;
	float angle;
	float angularVelocity;
	float angularAcceleration;
};

//速度や加速度地面の上にいるかどうか
struct PhysicsData {
	Vector3 velocity;
	Vector3 acceleration;
	bool isOnGround;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
};

//ボール
//position: 位置
//velocity: 速度
//acceleration: 加速度
//mass: 質量
//radius: 半径
struct Ball {
	float radius;
	Vector3 position;
	PhysicsData physicsData;
	float mass;
};