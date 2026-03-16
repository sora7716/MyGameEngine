#include "StageTimer.h"
#include "Text.h"
#include "ImGuiManager.h"
#include "algorithm/Math.h"
#include <sstream>
#include <iomanip>

//コンストラクタ
StageTimer::StageTimer() {
}

//デストラクタ
StageTimer::~StageTimer() {
}
//初期化
void StageTimer::Initialize(Object2dCommon* object2dCommon) {
	//文字スタイルを作成
	textStyle_.text = "\0";
	textStyle_.font = "\0";
	textStyle_.size = 16.0f;
	textStyle_.color = Vector4::MakeWhiteColor();
	//文字のトランスフォームデータを作成
	transformData_.scale = { 232.0f,200.0f };
	transformData_.rotate = 0.0f;
	transformData_.translate = { -390.0f,-150.0f };
	//文字の生成と初期化
	text_ = std::make_unique<Text>();
	text_->Initialize(object2dCommon, "stageTimerText");
	text_->SetTextStyle(textStyle_);
}

//更新
void StageTimer::Update() {
	//時間
	if (timer_ > 0.0f) {
		timer_ -= Math::kDeltaTime;
	} else {
		timer_ = 0.0f;
		isTimeUp_ = true;
	}

	//スコアの文字列を作成
	std::ostringstream scoreText;
	scoreText << "Time : " << std::setw(3) << std::setfill('0') << static_cast<int32_t>(timer_);
	text_->SetText(scoreText.str());
	text_->SetTextSize(textStyle_.size);
	text_->SetColor(textStyle_.color);
	text_->SetTransformDate(transformData_);
	//更新
	text_->Update();
}

//デバッグ
void StageTimer::Debug() {
#ifdef USE_IMGUI
	ImGui::DragFloat2("scale", &transformData_.scale.x, 0.1f);
	ImGui::DragFloat2("translate", &transformData_.translate.x, 0.1f);
	ImGui::DragFloat("textSize", &textStyle_.size, 0.1f);
	ImGui::ColorEdit4("color", &textStyle_.color.x);
#endif // USE_IMGUI
}

//描画
void StageTimer::Draw() {
	text_->Draw();
}

//時間切れのフラグのゲッター
bool StageTimer::IsTimeUp() {
	return isTimeUp_;
}
