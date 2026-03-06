#include "Camera.h"
#include "engine/math/func/Math.h"
#include "engine/base/WinApi.h"

/// <summary>
/// コンストラクタ
/// </summary>
Camera::Camera() {
	transform_ = { Vector3::MakeAllOne(),Quaternion::IdentityQuaternion(),{0.0f,0.0f,-10.0f}};
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

//クォータニオンのセッター
void Camera::SetQuaternion(const Quaternion& quaternion) {
	transform_.quaternion = quaternion;
}

// 平行移動のセッター
void Camera::SetTranslate(const Vector3& translate) {
	transform_.translate = translate;
}

// 水平方向視野角のセッター
void Camera::SetFovY(const float fovY) {
	fovY_ = fovY;
}

// アスペクト比のセッター
void Camera::SetAspectRation(const float aspectRation) {
	aspectRation_ = aspectRation;
}

// ニアクリップ距離のセッター
void Camera::SetNearClip(const float nearClip) {
	nearClip_ = nearClip;
}

// ファークリップ距離のセッター
void Camera::SetFarClip(const float farClip) {
	farClip_ = farClip;
}

// ワールド行列のゲッター
const Matrix4x4& Camera::GetWorldMatrix() const {
	// TODO: return ステートメントをここに挿入します
	return worldMatrix_;
}

// ビュー行列のゲッター
const Matrix4x4& Camera::GetViewMatrix() const {
	// TODO: return ステートメントをここに挿入します
	return viewMatrix_;
}

// 透視投影行列のゲッター
const Matrix4x4& Camera::GetProjectionMatrix() const {
	// TODO: return ステートメントをここに挿入します
	return projectionMatrix_;
}

// ビュープロジェクション行列
const Matrix4x4& Camera::GetViewProjectionMatrix() const {
	// TODO: return ステートメントをここに挿入します
	return viewProjectionMatrix_;
}

// 回転のゲッター
const Quaternion& Camera::GetQuaternion() const {
	// TODO: return ステートメントをここに挿入します
	return transform_.quaternion;
}

// 平行移動のゲッター
const Vector3& Camera::GetTranslate() const {
	// TODO: return ステートメントをここに挿入します
	return transform_.translate;
}

//ニアクリップ距離のゲッター
const float Camera::GetNearClip() const {
	return nearClip_;
}

//ファークリップ距離のゲッター
const float Camera::GetFarClip() const {
	return farClip_;
}

//FovYのゲッター
const float Camera::GetFovY() const {
	return fovY_;
}
