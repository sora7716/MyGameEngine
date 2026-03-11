#pragma once
#include "RenderingData.h"

//タグ
enum class Tag {
	kPlayer,
	kEnemy,
	kWall,
	kGround,
	kGoal,
	kItem,
	kNone
};

//ゲームオブジェクト
struct GameObject {
	TransformData transformData;
	bool isAlive;
	Tag tag;
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
};