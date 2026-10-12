#pragma once
#include <string>
#include <vector>
#include <variant>
//浮動小数のプロパティ
struct FloatProperty{
	std::string displayName = "";
	float* value = nullptr;
};

//フラグのプロパティ
struct BoolProperty{
	std::string displayName = "";
	bool* value = nullptr;
};

using Property = std::variant<FloatProperty, BoolProperty>;

//プロパティをまとめる
class PropertyRegistry{
public://メンバ関数
private://メンバ変数
};