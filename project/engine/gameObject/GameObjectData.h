#pragma once
#include "RenderingData.h"

//タグ
enum class Tag {
	kPlayer,
	kJumpPad,
	kGround,
	kGoal,
	kNone
};

//ゲームオブジェクト
struct GameObject {
	Transform transformData;
	bool isActive;
	bool isEnabled;
	Tag tag;
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
};