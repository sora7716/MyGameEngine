#include "RandomUtility.h"

//コンストラクタ
RandomUtility::RandomUtility() {
}

//デストラクタ
RandomUtility::~RandomUtility() {
}

//初期化
void RandomUtility::Initialize() {
	//乱数エンジンの初期化
	std::random_device seedGenerator;
	randomEngine_.seed(seedGenerator());
}
