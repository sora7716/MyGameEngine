#pragma once
#include "Vector4.h"
#include <string>
//テキストの生成時に必要なもの
//text:入力する文字
//font:使用するフォント
//size:テキストのサイズ
//color:テキストの色
struct TextStyle {
	std::string text;
	std::string font;
	float size;
	Vector4 color;
};