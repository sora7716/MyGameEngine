#include "Camera.h"
#include "WinApi.h"
#include "MathUtility.h"
#include "MatrixUtility.h"
#include "GameObject.h"

//コンストラクタ
Camera::Camera(GameObject* gameObject) :Component(gameObject) {
}

//デストラクタ
Camera::~Camera() {
}

//初期化
void Camera::Initialize() {
	//基底クラスの初期化
	Component::Initialize();
	gameObject_ = GetOwner();
	fovY_ = 0.45f;
	aspectRation_ = float(WinApi::kClientWidth) / float(WinApi::kClientHeight);
	nearClip_ = 0.1f;
	farClip_ = 100.0f;
	//視錐台のローカルの頂点を作成
	frustum_.localCorners = mathUtility::CreateFrustumVertex(nearClip_,farClip_,fovY_,aspectRation_);
	//デバッグの場合フラスタムかリングを行わない
#ifdef _DEBUG
	isFrustumCulling_ = false;
#else 
	isFrustumCulling_ = true;
#endif // _DEBUG

}

//更新
void Camera::Update() {
	//基底クラスの更新
	Component::Update();
	//worldMatrixの逆行列
	viewMatrix_ = gameObject_->CalculateWorldMatrix().Inverse();
	//透視投影行列の作成
	projectionMatrix_ = matrixUtility::MakePerspectiveFovMatrix(fovY_,aspectRation_,nearClip_,farClip_);
	//ビュープロジェクション行列の作成
	viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;

	//GPUに送信する用のポインタを保存
	renderData_.viewProjection = viewProjectionMatrix_;
	renderData_.worldPosition = gameObject_->GetWorldPos();

	//視錐台のローカルの頂点を作成
	frustum_.localCorners = mathUtility::CreateFrustumVertex(nearClip_,farClip_,fovY_,aspectRation_);
	//視錐台のデータを作成
	frustum_ = mathUtility::CreateFrustumData(frustum_.localCorners,gameObject_->CalculateWorldMatrix());

}

//複製
std::unique_ptr<Component> Camera::Clone(GameObject* gameObject) const {
	std::unique_ptr<Camera>cloneInstance = std::make_unique<Camera>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//Cameraが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	cloneInstance->SetFovY(fovY_);
	cloneInstance->SetNearClip(nearClip_);
	cloneInstance->SetFarClip(farClip_);
	cloneInstance->SetAspectRation(aspectRation_);
	return cloneInstance;
}

//プロパティ一覧の登録
void Camera::RegisterProperties(PropertyRegistry* registry) {
	Component::RegisterProperties(registry);
	registry->Add("視錐台カリングを行うか",isFrustumCulling_);
	registry->Add("fovY",fovY_,0.01f);
	registry->Add("アスペクト比",aspectRation_);
	registry->Add("近平面",nearClip_,0.1f);
	registry->Add("遠平面",farClip_,0.1f);
}

//オイラー角の設定
void Camera::SetEulerAngle(const Vector3& eulerAngle) {
	gameObject_->GetTransform().SetEulerAngle(eulerAngle);
}

//クォータニオンの設定
void Camera::SetQuaternion(const Quaternion& quaternion) {
	gameObject_->GetTransform().rotate = quaternion;
}

// 平行移動の設定
void Camera::SetTranslate(const Vector3& translate) {
	gameObject_->GetTransform().translate = translate;
}

// 垂直方向視野角の設定
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

//コンポーネントのタイプ名を取得
std::string Camera::GetTypeName() const {
	return "Camera";
}

// ビュー行列の取得
const Matrix4x4& Camera::GetViewMatrix() const {
	return viewMatrix_;
}

// 透視投影行列の取得
const Matrix4x4& Camera::GetProjectionMatrix() const {
	return projectionMatrix_;
}

// ビュープロジェクション行列
const Matrix4x4& Camera::GetViewProjectionMatrix() const {
	return viewProjectionMatrix_;
}

//視錐台の取得
primitiveData::Frustum& Camera::GetFrustum() {
	return frustum_;
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

//描画データの取得
const CameraRenderData& Camera::GetRenderData() {
	return renderData_;
}

//フラスタムかリングを行うかの取得
bool Camera::IsFrustumCulling() const {
	return isFrustumCulling_;
}
