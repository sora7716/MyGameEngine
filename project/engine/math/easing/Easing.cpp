#include "Easing.h"
#include <cmath>

//EaseInOutExpoの計算
float easing::EaseInOutExpo(float t,float sharpness) {
	if(t == 0.0f) {
		return  0.0f;
	} else if(t == 1.0f) {
		return  1.0f;
	}


	//だんだん加速する
	if(t < 0.5f) {
		return std::pow(2.0f,(sharpness * 2.0f) * t - sharpness) / 2.0f;
	}

	//だんだん減速する
	return (2.0f - std::pow(2.0f,-(sharpness * 2.0f) * t + sharpness)) / 2.0f;
}
