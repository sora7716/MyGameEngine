#include "FixFPS.h"
#include <thread>

//インスタンスのゲッター
FixFPS* FixFPS::GetInstance() {
	static FixFPS instance;
	return &instance;
}

//初期化
void FixFPS::Initialize() {
	//現在時間を記録する
	reference_ = std::chrono::steady_clock::now();
}

//更新
void FixFPS::Update() {
	//1/60秒ぴったりの時間
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
	//1/60秒よりわずかに短い時間
	const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));

	//現在の時間を取得
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	//前回の記録から経過時間を取得する
	std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	//1/60秒(よりわずかに短い時間)経っていない場合
	if(elapsed < kMinCheckTime) {
		//1/60秒経過するまで微小なスリープを繰り返す
		while(std::chrono::steady_clock::now() - reference_ < kMinTime) {
			//1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}

	//待ち終わった時間を取得数r
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	//1フレームにかかった時間(処理+待ち)
	std::chrono::microseconds frameTime = std::chrono::duration_cast<std::chrono::microseconds>(end - reference_);
	//FPSを記録
	if(frameTime.count() > 0) {
		fps_ = 1000000.0f / static_cast<float>(frameTime.count());
	}

	//現在の時間を記録する
	reference_ = end;

	//デルタタイムを記録する
	deltaTime_ = static_cast<float>(frameTime.count()) / 1000000.0f;
	//値がブレークポイントとかで大きくなりすぎないようにする
	const float kMaxDeltaTime = 1.0f / 30.0f;
	deltaTime_ = std::min(deltaTime_,kMaxDeltaTime);
}

//FPSの取得
float FixFPS::GetFPS() const {
	return fps_;
}

//デルタタイムの取得
float FixFPS::GetDeltaTime() const {
	return deltaTime_;
}

//コンストラクタ
FixFPS::FixFPS() {
}

//デストラクタ
FixFPS::~FixFPS() {
}
