#include "Camera.h"
#include "algorithms/Math.h"
#include "WinApi.h"

/// <summary>
/// コンストラクタ
/// </summary>
Camera::Camera() {
	transform_.Initialize();
	transform_.translate.z = -10.0f;
	fovY_ = 0.45f;
	aspectRation_ = float(WinApi::kClientWidth) / float(WinApi::kClientHeight);
	nearClip_ = 0.1f;
	farClip_ = 100.0f;
}

//更新
void Camera::Update() {
	//アフィン変換行列の作成
	worldMatrix_ = Rendering::MakeAffineMatrix(transform_);
	//worldMatrixの逆行列
	viewMatrix_ = worldMatrix_.Inverse();
	//透視投影行列の作成
	projectionMatrix_ = Rendering::MakePerspectiveFovMatrix(fovY_, aspectRation_, nearClip_, farClip_);
	//ビュープロジェクション行列の作成
	viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;
}

//オイラー角の設定
void Camera::SetEulerAngle(const Vector3& eulerAngle){
	transform_.eulerAngle = eulerAngle;
	transform_.quaternion = Quaternion::MakeQuaternionForEulerAngle(transform_.eulerAngle);
}

//クォータニオンの設定
void Camera::SetQuaternion(const Quaternion& quaternion) {
	transform_.quaternion = quaternion;
}

// 平行移動の設定
void Camera::SetTranslate(const Vector3& translate) {
	transform_.translate = translate;
}

// 水平方向視野角の設定
void Camera::SetFovY(const float fovY) {
	fovY_ = fovY;
}

// アスペクト比の設定
void Camera::SetAspectRation(const float aspectRation) {
	aspectRation_ = aspectRation;
}

// ニアクリップ距離の設定
void Camera::SetNearClip(const float nearClip) {
	nearClip_ = nearClip;
}

// ファークリップ距離の設定
void Camera::SetFarClip(const float farClip) {
	farClip_ = farClip;
}

// ワールド行列の取得
const Matrix4x4& Camera::GetWorldMatrix() const {
	// TODO: return ステートメントをここに挿入します
	return worldMatrix_;
}

//オイラー角の取得
const Vector3& Camera::GetEulerAngle() const {
	// TODO: return ステートメントをここに挿入します
	return transform_.eulerAngle;
}

// ビュー行列の取得
const Matrix4x4& Camera::GetViewMatrix() const {
	// TODO: return ステートメントをここに挿入します
	return viewMatrix_;
}

// 透視投影行列の取得
const Matrix4x4& Camera::GetProjectionMatrix() const {
	// TODO: return ステートメントをここに挿入します
	return projectionMatrix_;
}

// ビュープロジェクション行列
const Matrix4x4& Camera::GetViewProjectionMatrix() const {
	// TODO: return ステートメントをここに挿入します
	return viewProjectionMatrix_;
}

// 回転の取得
const Quaternion& Camera::GetQuaternion() const {
	// TODO: return ステートメントをここに挿入します
	return transform_.quaternion;
}

// 平行移動の取得
const Vector3& Camera::GetTranslate() const {
	// TODO: return ステートメントをここに挿入します
	return transform_.translate;
}

//ニアクリップ距離の取得
const float Camera::GetNearClip() const {
	return nearClip_;
}

//ファークリップ距離の取得
const float Camera::GetFarClip() const {
	return farClip_;
}

//FovYの取得
const float Camera::GetFovY() const {
	return fovY_;
}

//アスペクト比の取得
const float Camera::GetAspectRation() const {
	return aspectRation_;
}
