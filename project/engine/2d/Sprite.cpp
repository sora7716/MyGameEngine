#include "Sprite.h"
#include "SpriteCommon.h"
#include <cassert>
#include "algorithm/Math.h"
#include "TextureManager.h"
#include "DirectXBase.h"
#include "WinApi.h"
#include "WorldTransform.h"
#include "ImGuiManager.h"

//デストラクタ
Sprite::~Sprite() {
	delete worldTransform_;
}

//初期化
void Sprite::Initialize(SpriteCommon* spriteCommon, const std::string& spriteName) {
	spriteCommon_ = spriteCommon;
	directXBase_ = spriteCommon_->GetDirectXBase();//DirectXの基盤部分を受け取る
	//頂点データの生成
	CreateVertexResource();
	//インデックスデータの生成
	CreateIndexResource();
	//マテリアルデータの生成
	CreateMaterialResource();
	//スプライトファイルパスを記録
	spriteName_ = "engine/resources/textures/" + spriteName;
	//スプライトの共通部分
	spriteCommon_->LoadTexture(spriteName_);
	//スクリーンに表示する範囲を設定
	WorldTransform::ScreenArea screenArea = {
		.left = 0,
		.top = 0,
		.right = (float)WinApi::kClientWidth,
		.bottom = (float)WinApi::kClientHeight,
	};
	//ワールドトランスフォーム
	worldTransform_ = new WorldTransform();
	worldTransform_->Initialize(directXBase_, TransformMode::k2d);
	worldTransform_->SetScreenArea(screenArea);
}

//更新
void Sprite::Update() {
	worldTransform_->SetTransformData(transform_);
	//ワールドトランスフォームの更新
	worldTransform_->Update();
}

//描画処理
void Sprite::Draw() {
	//描画準備
	spriteCommon_->DrawSetting();
	//PSOの設定
	auto pso = spriteCommon_->GetGraphicsPipelineStates()[static_cast<int32_t>(blendMode_)].Get();
	//グラフィックスパイプラインをセットするコマンド
	directXBase_->GetCommandList()->SetPipelineState(pso);
	//ワールドトランスフォームの描画
	worldTransform_->Draw();
	//VertexBufferViewの設定
	directXBase_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
	//IndexBufferViewを設定
	directXBase_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);//IBVを設定
	//マテリアルCBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());//material
	//SRVのDescriptorTableの先頭を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(2, spriteCommon_->GetTextureManager()->GetSRVHandleGPU(spriteName_));
	//描画(DrawCall/ドローコール)
	directXBase_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

//テキストのセッター
void Sprite::SetText(const std::string& textName) {
	spriteName_ = textName;
}

//テクスチャの変更
void Sprite::ChangeTexture(const std::string& spriteName) {
	spriteName_ = "engine/resources/textures/" + spriteName;
	spriteCommon_->LoadTexture(spriteName_);
}

// UVの座標変換の更新
void Sprite::UpdateUVTransform(Transform2dData uvTransform) {
	//UVTransform
	materialData_->uvMatrix = Rendering::MakeUVAffineMatrix(uvTransform.scale, uvTransform.rotate, uvTransform.translate);
}

//色のゲッター
const Vector4& Sprite::GetColor() const {
	// TODO: return ステートメントをここに挿入します
	return materialData_->color;
}

//色のセッター
void Sprite::SetColor(const Vector4& color) {
	materialData_->color = color;
}

//トランスフォームのセッター
void Sprite::SetTransformData(const Transform2dData& transform) {
	transform_.scale.x = transform.scale.x;
	transform_.scale.y = transform.scale.y;
	transform_.quaternion.z = transform.rotate;
	transform_.translate.x = transform.translate.x;
	transform_.translate.y = transform.translate.y;
}

//ブレンドモードのセッター
void Sprite::SetBlendMode(BlendMode blendMode) {
	blendMode_ = blendMode;
}

//頂点データの初期化
void Sprite::InitializeVertexData() {
	//矩形
	vertexData_[0].position = { 0.0f,1.0f,0.0f,1.0f };//左下
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[0].normal = { 0.0f,0.0f,-1.0f };

	vertexData_[1].position = { 0.0f,0.0f,0.0f,1.0f };//左上
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[1].normal = { 0.0f,0.0f,-1.0f };

	vertexData_[2].position = { 1.0f,1.0f,0.0f,1.0f };//右下
	vertexData_[2].texcoord = { 1.0f,1.0f };
	vertexData_[2].normal = { 0.0f,0.0f,-1.0f };

	vertexData_[3].position = { 1.0f,0.0f,0.0f,1.0f };//右上
	vertexData_[3].texcoord = { 1.0f,0.0f };
	vertexData_[3].normal = { 0.0f,0.0f,-1.0f };
}

//頂点データの生成
void Sprite::CreateVertexResource() {
	//VertexResourceを作成する
	vertexResource_ = directXBase_->CreateBufferResource(sizeof(VertexData) * 6);
	//VertexBufferViewを作成する
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//VertexResorceにデータを書き込むためのアドレスを取得してvertexDataに割り当てる
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	InitializeVertexData();
}

//インデックスデータの初期化
void Sprite::InitializeIndexData() {
	indexData_[0] = 0; indexData_[1] = 1; indexData_[2] = 2;
	indexData_[3] = 1; indexData_[4] = 3; indexData_[5] = 2;
}

//インデックスリソースの生成
void Sprite::CreateIndexResource() {
	//IndexResourceを作成する
	indexResource_ = directXBase_->CreateBufferResource(sizeof(uint32_t) * 6);
	//IndexBufferViewを作成する
	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	//IndexResourceにデータを書き込むためのアドレスを取得してindexDataに割り当てる
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	InitializeIndexData();
}

//マテリアルデータの初期化
void Sprite::InitializeMaterialData() {
	//色を書き込む
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = false;
	materialData_->uvMatrix = Matrix4x4::Identity4x4();
}

//マテリアルリソースの生成
void Sprite::CreateMaterialResource() {
	//マテリアルリソースを作る
	materialResource_ = directXBase_->CreateBufferResource(sizeof(Material));
	//マテリアルリソースにデータを書き込むためのアドレスを取得してmaterialDataに割り当てる
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//マテリアルデータの初期値を書き込む
	InitializeMaterialData();
}