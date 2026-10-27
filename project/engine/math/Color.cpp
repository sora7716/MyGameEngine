#include "Color.h"
#include <cassert>

//RGBの取得
const RGB Color::GetRGB()const{
	return { r,g,b };
}

//RGBの設定
void Color::SetRGB(const RGB& rgb){
	r= rgb.r;
	g = rgb.g;
	b = rgb.b;
}

//白の取得
Color Color::GetWhite(){
	return Color(1.0f, 1.0f, 1.0f, 1.0f);
}

//赤の取得
Color Color::GetRed(){
	return Color(1.0f, 0.0f, 0.0f, 1.0f);
}

//緑の取得
Color Color::GetGreen(){
	return Color(0.0f, 1.0f, 0.0f, 1.0f);
}

//青の取得
Color Color::GetBlue(){
	return Color(0.0f, 0.0f, 1.0f, 1.0f);
}

//黒の取得
Color Color::GetBlack(){
	return Color(0.0f, 0.0f, 0.0f, 1.0f);
}

//カラーコードをColorに変換
Color Color::ColorCodeTransform(const std::string& colorCode){
	int32_t r = std::stoi(colorCode.substr(1, 2), nullptr, 16);
	int32_t g = std::stoi(colorCode.substr(3, 2), nullptr, 16);
	int32_t b = std::stoi(colorCode.substr(5, 2), nullptr, 16);
	int32_t alpha = (colorCode.size() == 9) ? std::stoi(colorCode.substr(7, 2), nullptr, 16) : 255;
	return Color(r / 255.0f, g / 255.0f, b / 255.0f, alpha / 255.0f);
}

//加法
Color Color::operator+(const Color& v){
	Color result = {};
	result.r = r + v.r;
	result.g = g + v.g;
	result.b = b + v.b;
	result.a = a + v.a;
	return result;
}

//除算
Color Color::operator/(float num){
	Color result = {};
	result.r = r / num;
	result.g = g / num;
	result.b = b / num;
	result.a = a / num;
	return result;
}

//スカラー倍
void Color::operator*=(float n){
	r *= n;
	g *= n;
	b *= n;
	a *= n;
}

//乗算
void Color::operator+=(const Color& v){
	*this = *this + v;
}
