#include "Score.h"
#include <sstream>
#include <iomanip>
#include "engine/2d/Text.h"
#include "engine/debug/ImGuiManager.h"

//デストラクタ
Score::~Score() {
	delete text_;
}

//スコアを加算
void Score::AddScore(int32_t addScore) {
	score += addScore;
}

//初期化
void Score::Initialize(Object2dCommon* object2dCommon) {
	//文字スタイルを作成
	textStyle_.text = "\0";
	textStyle_.font = "\0";
	textStyle_.size = 32.0f;
	textStyle_.color = Vector4::MakeWhiteColor();
	//文字のトランスフォームデータを作成
	transformData_.scale = { 232.0f,200.0f };
	transformData_.rotate = 0.0f;
	transformData_.translate = { -400.0f,-130.0f };
	//文字の生成と初期化
	text_ = new Text();
	text_->Initialize(object2dCommon, "scoreText");
	text_->SetTextStyle(textStyle_);
}

//更新
void Score::Update() {
	//スコアの文字列を作成
	std::ostringstream scoreText;
	scoreText << "SCORE : " << std::setw(kDigitCount) << std::setfill('0') << score;
	text_->SetText(scoreText.str());
	text_->SetTextSize(textStyle_.size);
	text_->SetColor(textStyle_.color);
	text_->SetTransformDate(transformData_);
	//更新
	text_->Update();
}

//デバッグ
void Score::Debug() {
#ifdef USE_IMGUI
	ImGui::DragFloat2("scale", &transformData_.scale.x, 0.1f);
	ImGui::DragFloat2("translate", &transformData_.translate.x, 0.1f);
	ImGui::DragFloat("textSize", &textStyle_.size, 0.1f);
	ImGui::ColorEdit4("color", &textStyle_.color.x);
#endif // USE_IMGUI
}

//描画
void Score::Draw() {
	text_->Draw();
}

//位置のセッター
void Score::SetPosition(const Vector2& position) {
	transformData_.translate = position;
}

//スケールのセッター
void Score::SetScale(const Vector2& scale) {
	transformData_.scale = scale;
}

//色のセッター
void Score::SetColor(const Vector4& color) {
	textStyle_.color = color;
}

//テキストのサイズのセッター
void Score::SetTextSize(float size) {
	textStyle_.size = size;
}

//スコアのセッター
void Score::SetScore(int32_t score) {
	this->score = score;
}

//スコアのゲッター
int32_t Score::GetScore() const {
	return score;
}


