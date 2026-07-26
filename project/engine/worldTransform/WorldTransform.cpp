#include "WorldTransform.h"
#include "DirectXBase.h"
#include "MatrixUtility.h"
#include "MathUtility.h"
#include "Camera.h"
#include <cmath>
//メンバ関数テーブルの初期化
void(WorldTransform::* WorldTransform::UpdateTransformTable[])() = {
	&UpdateTransform,
	&UpdateTransform2d,
	&UpdateTransformBillboard,
	&UpdateTrasformDirectionToDirection
};

//コンストラクタ
WorldTransform::WorldTransform(){
}

//デストラクタ
WorldTransform::~WorldTransform(){
}

//初期化
void WorldTransform::Initialize(DirectXBase* directXBase, TransformMode transformMode){
	//DirectXの基盤部分の記録
	directXBase_ = directXBase;
	//ワールド座標
	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	//wvpリソースの初期化
	CreateTransformationMatrixResource();
	transformMode_ = transformMode;
}

//更新
void WorldTransform::Update(){
	//トランスフォームの更新
	(this->*UpdateTransformTable[static_cast<uint32_t>(transformMode_)])();
}

//描画
void WorldTransform::Draw(){
	//座標変換行列CBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());//wvp
}

//スクリーンに表示する範囲
void WorldTransform::SetScreenArea(ScreenArea screenArea){
	screenArea_ = screenArea;
}

//カメラのセッター
void WorldTransform::SetCamera(Camera* camera){
	camera_ = camera;
}

//親のセッター
void WorldTransform::SetParent(const WorldTransform* parent){
	parent_ = parent;
}

//親子付け
void WorldTransform::Compose(const WorldTransform* parent){
	Matrix4x4 childLocal = parent_->worldMatrix_.Inverse() * worldMatrix_;
	transform_ = matrixUtility::DecomposeMatrix(childLocal);
	SetParent(parent);
}

//親子関係を解除
void WorldTransform::Decompose(){
	if (parent_){
		//ワールド行列をTransformDataに分解
		transform_ = matrixUtility::DecomposeMatrix(worldMatrix_);
		//親子関係を解除
		parent_ = nullptr;
	}
}

//ワールド座標のセッター
void WorldTransform::SetTransformData(const Transform& transform){
	transform_ = transform;
}

//ワールド座標のセッター(2D)
void WorldTransform::SetTransform2d(const Transform2d& transform2d){
	transform_.scale = { transform2d.scale.x,transform2d.scale.y,1.0f };
	transform_.eulerAngle = { 0.0f,0.0f,transform2d.rotate };
	transform_.quaternion = Quaternion::MakeQuaternionForEulerAngle(transform_.eulerAngle);
	transform_.translate = { transform2d.translate.x,transform2d.translate.y,0.0f };
}

//スケールのセッター
void WorldTransform::SetScale(const Vector3& scale){
	transform_.scale = scale;
}

//回転のセッター
void WorldTransform::SetRotate(const Vector3& rotate){
	transform_.quaternion = { rotate.x,rotate.y,rotate.z,0.0f };
}

//平行移動のセッター
void WorldTransform::SetTranslate(const Vector3& translate){
	transform_.translate = translate;
}

//ワールドマトリックスのセッター
void WorldTransform::SetWorldMatrix(const Matrix4x4& worldMatrix){
	worldMatrix_ = worldMatrix;
}

//ノードのセッター
void WorldTransform::SetNode(const Node& node){
	node_ = node;
}

//ワールド行列のゲッター
const Matrix4x4& WorldTransform::GetWorldMatrix() const{
	// TODO: return ステートメントをここに挿入します
	return worldMatrix_;
}

//スケールのゲッター
const Vector3& WorldTransform::GetScale() const{
	// TODO: return ステートメントをここに挿入します
	return transform_.scale;
}

//回転のゲッター
const Quaternion& WorldTransform::GetQuaternion() const{
	// TODO: return ステートメントをここに挿入します
	return transform_.quaternion;
}

//平行移動のセッター
const Vector3& WorldTransform::GetTranslate() const{
	// TODO: return ステートメントをここに挿入します
	return transform_.translate;
}

//トランスフォームデータのゲッター
const Transform& WorldTransform::GetTransform() const{
	// TODO: return ステートメントをここに挿入します
	return transform_;
}

//カメラのゲッター
Camera* WorldTransform::GetCamera(){
	return camera_;
}

//ワールド座標のゲッター
Vector3 WorldTransform::GetWorldPos(){
	Vector3 result = { worldMatrix_.m[3][0],worldMatrix_.m[3][1],worldMatrix_.m[3][2] };
	return result;
}

//FromPosとToPosのセッター
void WorldTransform::SetFromAndToPos(const Vector3& fromPos, const Vector3& toPos){
	fromPos_ = fromPos;
	toPos_ = toPos;
}

//座標変換行列リソースの生成
void WorldTransform::CreateTransformationMatrixResource(){
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
void WorldTransform::UpdateTransform(){
	worldMatrix_ = matrixUtility::MakeAffineMatrix(transform_);
	//TransformからWorldMatrixを作る
	if (parent_){
		worldMatrix_ = worldMatrix_ * parent_->worldMatrix_;
	}
	//wvpの書き込み
	if (camera_){
		const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
		wvpData_->wvp = node_.localMatrix * worldMatrix_ * viewProjectionMatrix;
	} else{
		wvpData_->wvp = worldMatrix_;
	}
	//ワールド行列を送信
	wvpData_->world = node_.localMatrix * worldMatrix_;
	//逆行列の転置行列を送信
	wvpData_->worldInverseTranspose = worldMatrix_.InverseTranspose();
}

//座標の更新(向きたい方向に向かせる)
void WorldTransform::UpdateTrasformDirectionToDirection(){
	Matrix4x4 scaleMat = matrixUtility::MakeScaleMatrix(transform_.scale);
	Matrix4x4 rotateMat = matrixUtility::DirectionToDirection(fromPos_, toPos_);
	Matrix4x4 translateMat = matrixUtility::MakeTranslateMatrix(transform_.translate);
	worldMatrix_ = scaleMat * rotateMat * translateMat;
	//TransformからWorldMatrixを作る
	if (parent_){
		worldMatrix_ = worldMatrix_ * parent_->worldMatrix_;
	}
	//wvpの書き込み
	if (camera_){
		const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
		wvpData_->wvp = node_.localMatrix * worldMatrix_ * viewProjectionMatrix;
	} else{
		wvpData_->wvp = worldMatrix_;
	}
	//ワールド行列を送信
	wvpData_->world = node_.localMatrix * worldMatrix_;
	//逆行列の転置行列を送信
	wvpData_->worldInverseTranspose = worldMatrix_.InverseTranspose();
}

//座標の更新(2次元)
void WorldTransform::UpdateTransform2d(){
	//TransformからWorldMatrixを作る
	worldMatrix_ = matrixUtility::MakeAffineMatrix(transform_);
	if (parent_){
		worldMatrix_ = worldMatrix_ * parent_->worldMatrix_;
	}
	//ProjectionMatrixを作って平行投影行列を書き込む
	const Matrix4x4& projectionMatrix = matrixUtility::MakeOrthographicMatrix(screenArea_.left, screenArea_.top, screenArea_.right, screenArea_.bottom, 0.1f, 100.0f);
	//wvpの書き込み
	const Matrix4x4& viewProjectionMatrix = Matrix4x4::Identity4x4() * projectionMatrix;
	wvpData_->wvp = worldMatrix_ * viewProjectionMatrix;
	/*if (camera_) {

	}
	else {
		wvpData_->WVP = worldMatrix_ * projectionMatrix;
	}*/
	//ワールド行列を送信
	wvpData_->world = worldMatrix_;
}

//ビルボード行列での更新
void WorldTransform::UpdateTransformBillboard(){
	//カメラがなかったら
	if (!camera_){
		wvpData_->wvp = worldMatrix_;
		return;
	}
	worldMatrix_ = matrixUtility::MakeBillboardAffineMatrix(camera_->GetWorldMatrix(), transform_);
	//TransformからWorldMatrixを作る
	if (parent_){
		worldMatrix_ = worldMatrix_ * parent_->worldMatrix_;
	}
	//wvpの書き込み
	const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
	wvpData_->wvp = worldMatrix_ * viewProjectionMatrix;
	//ワールド行列を送信
	wvpData_->world = worldMatrix_;
	//逆行列の転置行列を送信
	wvpData_->worldInverseTranspose = wvpData_->world.InverseTranspose();
}


