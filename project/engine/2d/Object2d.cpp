#include "Object2d.h"
#include "Object2dCommon.h"
#include "TextureManager.h"
#include "WorldTransform.h"
#include "DirectXBase.h"
#include "WinApi.h"
#include "algorithms/Rendering.h"
#include "algorithms/Math.h"
#include "Camera.h"

//デストラクタ
Object2d::~Object2d() {
	delete worldTransform_;
}
//初期化
void Object2d::Initialize(Object2dCommon* object2dCommon, const std::string& textureName) {
	////2Dオブジェクトの共通部分
	//object2dCommon_ = object2dCommon;
	////DirectXの基盤部分を記録する
	//directXBase_ = object2dCommon_->GetDirectXBase();
	////頂点データの生成
	//CreateVertexResource();
	////インデックスリソースの生成
	//CreateIndexResource();
	////マテリアルデータの生成
	//CreateMaterialResource();
	////テクスチャのファイルパスの記録
	//modelData_.material.textureFilePath = "engine/resources/textures/" + textureName;
	////テクスチャの読み込み
	//object2dCommon_->GetTextureManager()->LoadTexture(modelData_.material.textureFilePath);
	////uvTransform変数を作る
	//uvTransform_ = { {1.0f,1.0f},Math::kPi,{0.0f,0.0f} };
	////ワールドトランスフォームの生成と初期化
	//worldTransform_ = new WorldTransform();
	//worldTransform_->Initialize(directXBase_, TransformMode::k2d);
	////カメラにデフォルトカメラを代入
	//worldTransform_->SetCamera(object2dCommon_->GetDefaultCamera());
	////スクリーンに表示する範囲を設定
	//WorldTransform::ScreenArea screenArea = {
	//	.left = -static_cast<float>(WinApi::kClientWidth / 2),
	//	.top = -static_cast<float>(WinApi::kClientHeight / 2),
	//	.right = static_cast<float>(WinApi::kClientWidth / 2),
	//	.bottom = static_cast<float>(WinApi::kClientHeight / 2),
	//};
	//worldTransform_->SetScreenArea(screenArea);
}

//更新
void Object2d::Update() {
	//トランスフォームのセッター
	worldTransform_->SetTransform2d(transform_);
	//ワールドトランスフォームの更新
	worldTransform_->Update();
	//UVTransform
	materialData_->uvMatrix = Rendering::MakeUVAffineMatrix(uvTransform_);
}

//描画
void Object2d::Draw() {
	////2Dオブジェクトの共通部分
	//object2dCommon_->DrawSetting();
	////PSOの設定
	//auto pso = object2dCommon_->GetGraphicsPipelineStates()[static_cast<int32_t>(blendMode_)].Get();
	////グラフィックスパイプラインをセットするコマンド
	//directXBase_->GetCommandList()->SetPipelineState(pso);
	////ワールドトランスフォームの描画
	//worldTransform_->Draw();
	////IndexBufferViewの設定
	//directXBase_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);//IBVを設定
	////VertexBufferViewの設定
	//directXBase_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
	////マテリアルCBufferの場所を設定
	//directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());//material
	////SRVのDescriptorTableの先頭を設定
	//directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(2, object2dCommon_->GetTextureManager()->GetSRVHandleGPU(modelData_.material.textureFilePath));
	////描画(DrawCall/ドローコール)
	//directXBase_->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(modelData_.meshes.vertices.size()), 1, 0, 0, 0);
}

//テキストのセッター
void Object2d::SetText(const std::string& textName) {
	//modelData_.material.textureFilePath = textName;
}

//テクスチャの変更
void Object2d::ChangeTexture(std::string textureName) {
	//modelData_.material.textureFilePath = "engine/resources/textures/" + textureName;
	//object2dCommon_->GetTextureManager()->LoadTexture(modelData_.material.textureFilePath);
}

//サイズのゲッター
const Vector2& Object2d::GetScale() const {
	// TODO: return ステートメントをここに挿入します
	Vector2* result = {};
	*result = worldTransform_->GetScale();
	return *result;
}

//回転のゲッター
float Object2d::GetRotate() const {
	return worldTransform_->GetQuaternion().z;
}

//位置のゲッター
const Vector2& Object2d::GetTranslate() const {
	// TODO: return ステートメントをここに挿入します
	Vector2* result = {};
	*result = worldTransform_->GetTranslate();
	return *result;
}

//UVのサイズのゲッター
const Vector2& Object2d::GetUVScale() const {
	// TODO: return ステートメントをここに挿入します
	return uvTransform_.scale;
}

//UVの回転のゲッター
float Object2d::GetUVRotate() const {
	return uvTransform_.rotate;
}

//UVの位置のゲッター
const Vector2& Object2d::GetUVTranslate() const {
	// TODO: return ステートメントをここに挿入します
	return uvTransform_.translate;
}

//色のゲッター
const Vector4& Object2d::GetColor() const {
	return materialData_->color;
}

//ワールドトランスフォームのゲッター
WorldTransform* Object2d::GetWorldTransform() {
	return worldTransform_;
}


//サイズのセッター
void Object2d::SetScale(const Vector2& scale) {
	transform_.scale = scale;
}

//回転のセッター
void Object2d::SetRotate(float rotate) {
	transform_.rotate = rotate;
}

//位置のセッター
void Object2d::SetTranslate(const Vector2& translate) {
	transform_.translate = translate;
}

//トランスフォームのセッター
void Object2d::SetTransformData(const Transform2d& transform) {
	transform_ = transform;
}

//UVのサイズのセッター
void Object2d::SetUVScale(const Vector2& scale) {
	uvTransform_.scale = scale;
}

//UVの回転のセッター
void Object2d::SetUVRotate(float rotate) {
	uvTransform_.rotate = rotate;
}

//UVの位置のゲッター
void Object2d::SetUVTranslate(const Vector2& translate) {
	uvTransform_.translate = translate;
}

//色のセッター
void Object2d::SetColor(const Vector4& color) {
	materialData_->color = color;
}

//カメラのセッター
void Object2d::SetCamera(Camera* camera) {
	worldTransform_->SetCamera(camera);
}

//親のセッター
void Object2d::SetParent(const WorldTransform* parent) {
	worldTransform_->SetParent(parent);
}

//ブレンドモードのセッター
void Object2d::SetBlendMode(const BlendMode& blendMode) {
	blendMode_ = blendMode;
}

//頂点データの初期化
void Object2d::InitializeVertexData() {
	////左上
	//modelData_.meshes.vertices.push_back({ .position = {1.0f,1.0f,0.0f,1.0f},.texcoord = {0.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	////右上
	//modelData_.meshes.vertices.push_back({ .position = {-1.0f,1.0f,0.0f,1.0f},.texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	////左下
	//modelData_.meshes.vertices.push_back({ .position = {1.0f,-1.0f,0.0f,1.0f},.texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });

	////左下
	//modelData_.meshes.vertices.push_back({ .position = {1.0f,-1.0f,0.0f,1.0f},.texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });
	////右上
	//modelData_.meshes.vertices.push_back({ .position = {-1.0f,1.0f,0.0f,1.0f},.texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	////右下
	//modelData_.meshes.vertices.push_back({ .position = {-1.0f,-1.0f,0.0f,1.0f},.texcoord = {1.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });
}

//インデックスリソースの生成
void Object2d::CreateIndexResource() {
	////Index用(3dGameObject)
	//indexResource_ = directXBase_->CreateBufferResource(sizeof(uint32_t) * modelData_.meshes.vertices.size());
	////リソースの先頭のアドレスから使う
	//indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	////使用するリソースのサイズはインデックス6つ分のサイズ
	//indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * modelData_.meshes.vertices.size());
	////インデックスはuint32_tとする
	//indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	////IndexResourceにデータを書き込む
	//indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	//for (int i = 0; i < modelData_.meshes.vertices.size(); i++) {
	//	indexData_[i] = i; indexData_[i + 1] = i + 1; indexData_[i + 2] = i + 2;
	//	indexData_[i + 3] = i + 1; indexData_[i + 4] = i + 3; indexData_[i + 5] = i + 2;
	//}
}

//頂点データの生成
void Object2d::CreateVertexResource() {
	////頂点データの初期化
	//InitializeVertexData();
	////頂点リソースを生成
	//vertexResource_ = directXBase_->CreateBufferResource(sizeof(VertexData) * modelData_.meshes.vertices.size());
	////VertexBufferViewを作成する(頂点バッファービュー)
	////リソースの先頭アドレスから使う
	//vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	////使用するリソースのサイズは頂点3つ分のサイズ
	//vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.meshes.vertices.size());
	////1頂点当たりのサイズ
	//vertexBufferView_.StrideInBytes = sizeof(VertexData);

	////頂点リソースにデータを書き込む
	//VertexData* vertexData = nullptr;
	////書き込むためのアドレスを取得
	//vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));//書き込むためのアドレスを取得
	//std::memcpy(vertexData, modelData_.meshes.vertices.data(), sizeof(VertexData) * modelData_.meshes.vertices.size());//頂点データをリソースにコピー
}

//マテリアルデータの初期化
void Object2d::InitializeMaterialData() {
	//色を書き込む
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = false;
	materialData_->uvMatrix = Matrix4x4::Identity4x4();
}

//マテリアルリソースの生成
void Object2d::CreateMaterialResource() {
	//マテリアルリソースを作る
	materialResource_ = directXBase_->CreateBufferResource(sizeof(Material));
	//マテリアルリソースにデータを書き込むためのアドレスを取得してmaterialDataに割り当てる
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//マテリアルデータの初期値を書き込む
	InitializeMaterialData();
}