#include "BaseShape.h"
#include "DirectXBase.h"
#include "MatrixUtility.h"
#include "MathUtility.h"
#include "Logger.h"
#include "GraphicsPipeline.h"
#include "Camera.h"
#include "ImGuiManager.h"
using namespace Microsoft::WRL;
using namespace debugDraw;

#pragma comment(lib,"d3d12.lib")

//コンストラクタ
BaseShape::BaseShape() {}

//デストラクタ
BaseShape::~BaseShape() {}

//初期化
void BaseShape::Initialize(DirectXBase* directXBase, Camera* camera) {
	//DirectXの基盤部分を記録する
	directXBase_ = directXBase;
	//カメラの記録
	renderCamera_ = camera;

	//頂点データの生成
	CreateVertexResource();
	//インデックスリソースの生成
	CreateIndexResource();
	//マテリアルデータの生成
	CreateMaterialResource();

	transform_.Initialize();

	//wvpリソースの初期化
	CreateTransformationMatrixResource();
}

//更新
void BaseShape::Update() {
	//頂点データの設定
	SettingVertexData();
	//ワールドトランスフォームの更新
	UpdateTransform();

	//描画データのまとめる
	renderData_.blendMode = blendMode_;
	renderData_.indexBufferView = indexBufferView_;
	renderData_.vertexBufferView = vertexBufferView_;
	renderData_.materialResource = materialResource_;
	renderData_.wvpResource = wvpResource_;
	renderData_.renderCamera = renderCamera_;
	renderData_.indexCount = indexCount_;
}

//描画する用のカメラを設定
void BaseShape::SetRenderCamera(Camera* camera) {
	renderCamera_ = camera;
}

//色の設定
void BaseShape::SetColor(const Vector4& color) {
	*color_ = color;
}

//色の取得
Vector4 BaseShape::GetColor() {
	return *color_;
}

//描画データの取得
const DebugDrawRenderData& debugDraw::BaseShape::GetRenderData(){
	// TODO: return ステートメントをここに挿入します
	return renderData_;
}

//インデックスリソースの生成
void BaseShape::CreateIndexResource() {
	indexResource_ = directXBase_->CreateBufferResource(sizeof(uint32_t) * indexCount_);

	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * indexCount_);
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	//インデックスデータの設定
	SettingIndexData();
}

//頂点データの生成
void BaseShape::CreateVertexResource() {
	//頂点リソースを生成
	vertexResource_ = directXBase_->CreateBufferResource(sizeof(VertexData) * vertexCount_);
	//VertexBufferViewを作成する(頂点バッファービュー)
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * vertexCount_);
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));//書き込むためのアドレスを取得

	//頂点データの初期化
	SettingVertexData();
}

//マテリアルデータの初期化
void BaseShape::InitializeMaterialData() {
	//色を書き込む
	*color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
}

//マテリアルリソースの生成
void BaseShape::CreateMaterialResource() {
	//マテリアルリソースを作る
	materialResource_ = directXBase_->CreateBufferResource(sizeof(Vector4));
	//マテリアルリソースにデータを書き込むためのアドレスを取得してmaterialDataに割り当てる
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&color_));
	//マテリアルデータの初期値を書き込む
	InitializeMaterialData();
}

//WorldTransformation行列リソースの生成
void BaseShape::CreateTransformationMatrixResource() {
	//座標変換行列リソースを作成する
	wvpResource_ = directXBase_->CreateBufferResource(sizeof(TransformationMatrix));
	//座標変換行列リソースにデータを書き込むためのアドレスを取得してtransformationMatrixDataに割り当てる
	//書き込むためのアドレス
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	//単位行列を書き込んでおく
	wvpData_->wvp = Matrix4x4::Identity4x4();
	wvpData_->world = Matrix4x4::Identity4x4();
	wvpData_->worldInverseTranspose = Matrix4x4::Identity4x4();
}

//座標の更新
void BaseShape::UpdateTransform() {
	worldMatrix_ = matrixUtility::MakeAffineMatrix(transform_);
	//TransformからWorldMatrixを作る
	//if (parent_) {
	//	worldMatrix_ = worldMatrix_ * parent_->worldMatrix_;
	//}
	//wvpの書き込み
	if (renderCamera_) {
		const Matrix4x4& viewProjectionMatrix = renderCamera_->GetViewProjectionMatrix();
		wvpData_->wvp = worldMatrix_ * viewProjectionMatrix;
	} else {
		wvpData_->wvp = worldMatrix_;
	}
	//ワールド行列を送信
	wvpData_->world = worldMatrix_;
	//逆行列の転置行列を送信
	wvpData_->worldInverseTranspose = worldMatrix_.InverseTranspose();
}