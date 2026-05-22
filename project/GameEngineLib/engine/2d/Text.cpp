#define NOMINMAX
#include "Text.h"
#include "Object2dCommon.h"
#include "TextureManager.h"
#include "WorldTransform.h"
#include "DirectXBase.h"
#include "WinApi.h"
#include "algorithms/Rendering.h"
#include "algorithms/Math.h"
#include "Camera.h"
#include "StringUtility.h"
#include <algorithm>

//デストラクタ
Text::~Text() {
	Finalize();
}

//初期化
void Text::Initialize(Object2dCommon* object2dCommon, const std::string& textKey) {
	//2dオブジェクトの共通部分
	object2dCommon_ = object2dCommon;
	//DirectXの基盤部分を記録する
	directXBase_ = object2dCommon_->GetDirectXBase();
	//頂点データの生成
	CreateVertexResource();
	//インデックスリソースの生成
	CreateIndexResource();
	//マテリアルデータの生成
	CreateMaterialResource();
	//テキストラスタライザーの生成と初期化
	textRasterizer_ = new TextRasterizer();
	textRasterizer_->Initialize();
	//テキストの検索キーを記録
	textData_.textKey = textKey;
	//テキストを設定
	textStyle_.text = "Sample";
	textStyle_.font = "\0";
	textStyle_.size = 32.0f;
	textStyle_.color = Vector4::MakeWhiteColor();
	//uvTransform変数を作る
	uvTransform_ = { {1.0f,1.0f},Math::kPi,{0.0f,0.0f} };
	//ワールドトランスフォームの生成と初期化
	worldTransform_ = new WorldTransform();
	worldTransform_->Initialize(directXBase_, TransformMode::k2d);
	//カメラにデフォルトカメラを代入
	worldTransform_->SetCamera(object2dCommon_->GetDefaultCamera());
	//スクリーンに表示する範囲を設定
	WorldTransform::ScreenArea screenArea = {
		.left = -static_cast<float>(WinApi::kClientWidth / 2),
		.top = -static_cast<float>(WinApi::kClientHeight / 2),
		.right = static_cast<float>(WinApi::kClientWidth / 2),
		.bottom = static_cast<float>(WinApi::kClientHeight / 2),
	};
	worldTransform_->SetScreenArea(screenArea);
}

//更新
void Text::Update() {
	//トランスフォームのセッター
	worldTransform_->SetTransform2d(transform_);
	//ワールドトランスフォームの更新
	worldTransform_->Update();
	//UVTransform
	materialData_->uvMatrix = Rendering::MakeUVAffineMatrix(uvTransform_.scale, uvTransform_.rotate, uvTransform_.translate);
	//テキストのテクスチャの取得
	AcquireTextTexture();
}

//描画
void Text::Draw() {
	//2Dオブジェクトの共通部分
	object2dCommon_->DrawSetting();
	//PSOの設定
	auto pso = object2dCommon_->GetGraphicsPipelineStates()[static_cast<int32_t>(blendMode_)].Get();
	//グラフィックスパイプラインをセットするコマンド
	directXBase_->GetCommandList()->SetPipelineState(pso);
	//ワールドトランスフォームの描画
	worldTransform_->Draw();
	//IndexBufferViewの設定
	directXBase_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);//IBVを設定
	//VertexBufferViewの設定
	directXBase_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
	//マテリアルCBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());//material
	//SRVのDescriptorTableの先頭を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(2, object2dCommon_->GetTextureManager()->GetSRVHandleGPU(textData_.textKey));
	//描画(DrawCall/ドローコール)
	directXBase_->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(textData_.vertices.size()), 1, 0, 0, 0);
}

//終了
void Text::Finalize() {
	delete worldTransform_;
	delete textRasterizer_;
}

//スケールのセッター
void Text::SetScale(const Vector2& textRange) {
	transform_.scale = textRange;
}

//回転
void Text::SetRotate(float rotate) {
	transform_.rotate = rotate;
}

//平行移動
void Text::SetTranslate(const Vector2& translate) {
	transform_.translate = translate;
}

//トランスフォームデータのセッター
void Text::SetTransformDate(const Transform2dData& transformData) {
	transform_ = transformData;
}

//文字の大きさ
void Text::SetTextSize(float size) {
	textStyle_.size = size;
}

//表示するテキストのセッター
void Text::SetText(const std::string& text) {
	textStyle_.text = text;
}

//使用するフォントのセッター
void Text::SetFont(const std::string& font) {
	textStyle_.font = font;
}

//カラーのセッター
void Text::SetColor(const Vector4& color) {
	textStyle_.color = color;
}

//テキストスタイルのセッター
void Text::SetTextStyle(const TextStyle& textStyle) {
	textStyle_ = textStyle;
}

//カメラのセッター
void Text::SetCamera(Camera* camera) {
	worldTransform_->SetCamera(camera);
}

//頂点データの初期化
void Text::InitializeVertexData() {
	//左上
	textData_.vertices.push_back({ .position = {1.0f,1.0f,0.0f,1.0f},.texcoord = {0.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	//右上
	textData_.vertices.push_back({ .position = {-1.0f,1.0f,0.0f,1.0f},.texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	//左下
	textData_.vertices.push_back({ .position = {1.0f,-1.0f,0.0f,1.0f},.texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });

	//左下
	textData_.vertices.push_back({ .position = {1.0f,-1.0f,0.0f,1.0f},.texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });
	//右上
	textData_.vertices.push_back({ .position = {-1.0f,1.0f,0.0f,1.0f},.texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	//右下
	textData_.vertices.push_back({ .position = {-1.0f,-1.0f,0.0f,1.0f},.texcoord = {1.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });
}

//インデックスリソースの生成
void Text::CreateIndexResource() {
	//Index用(3dGameObject)
	indexResource_ = directXBase_->CreateBufferResource(sizeof(uint32_t) * textData_.vertices.size());
	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * textData_.vertices.size());
	//インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	//IndexResourceにデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	for (int i = 0; i < textData_.vertices.size(); i++) {
		indexData_[i] = i; indexData_[i + 1] = i + 1; indexData_[i + 2] = i + 2;
		indexData_[i + 3] = i + 1; indexData_[i + 4] = i + 3; indexData_[i + 5] = i + 2;
	}
}

//頂点データの生成
void Text::CreateVertexResource() {
	//頂点データの初期化
	InitializeVertexData();
	//頂点リソースを生成
	vertexResource_ = directXBase_->CreateBufferResource(sizeof(VertexData) * textData_.vertices.size());
	//VertexBufferViewを作成する(頂点バッファービュー)
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * textData_.vertices.size());
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));//書き込むためのアドレスを取得
	std::memcpy(vertexData, textData_.vertices.data(), sizeof(VertexData) * textData_.vertices.size());//頂点データをリソースにコピー
}

//マテリアルデータの初期化
void Text::InitializeMaterialData() {
	//色を書き込む
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = false;
	materialData_->uvMatrix = Matrix4x4::Identity4x4();
}

//マテリアルリソースの生成
void Text::CreateMaterialResource() {
	//マテリアルリソースを作る
	materialResource_ = directXBase_->CreateBufferResource(sizeof(Material));
	//マテリアルリソースにデータを書き込むためのアドレスを取得してmaterialDataに割り当てる
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//マテリアルデータの初期値を書き込む
	InitializeMaterialData();
}

//テキストのテクスチャを取得または生成
void Text::AcquireTextTexture() {
	//スケールが1未満にならないようにする
	transform_.scale.x = std::max(transform_.scale.x, 1.0f);
	transform_.scale.y = std::max(transform_.scale.y, 1.0f);
	textStyle_.size = std::max(textStyle_.size, 1.0f);

	//CPUBitmapを作成
	CpuBitmap cpuBitmap = textRasterizer_->RenderTextToCpuBitmap(StringUtility::ConvertString(textStyle_.text), static_cast<uint32_t>(transform_.scale.x), static_cast<uint32_t>(transform_.scale.y), StringUtility::ConvertString(textStyle_.font), textStyle_.size, textStyle_.color);

	//テクスチャマネージャーに登録
	object2dCommon_->GetTextureManager()->UpdateTextureFromMemoryBGRA(textData_.textKey, cpuBitmap.bgra.data(), cpuBitmap.width, cpuBitmap.height, cpuBitmap.stride);
}
