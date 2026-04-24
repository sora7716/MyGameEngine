#include "WireframeObject3d.h"
#include "WireframeObject3dCommon.h"
#include "DirectXBase.h"
#include "Camera.h"
#include "algorithms/Math.h"
#include "ModelManager.h"
#include "Model.h"
#include "algorithms/Rendering.h"
#include "ImGuiManager.h"
#include "SRVManager.h"
#include "TextureManager.h"
#include <cassert>
//メンバ関数テーブルの初期化
void(WireframeObject3d::* WireframeObject3d::UpdateTransformTable[])() = {
	&UpdateTransform,
	&UpdateTransformBillboard,
};

//コンストラクタ
WireframeObject3d::WireframeObject3d() {
}

//デストラクタ
WireframeObject3d::~WireframeObject3d() {
}

//初期化
void WireframeObject3d::Initialize(WireframeObject3dCommon* wireframeObject3dCommon, Camera* camera, ModelType modelType, uint32_t instanceCount, Transform3dMode transform3dMode) {
	//ワイヤーフレームオブジェクトの共通部分を記録
	wireframeObject3dCommon_ = wireframeObject3dCommon;
#ifdef _DEBUG
	//マテリアルの初期化
	material_.color = { 0.0f,0.0f,0.0f,1.0f };
	material_.enableLighting = false;
	material_.uvMatrix = Matrix4x4::Identity4x4();
	material_.shininess = 10.0f;

	//モデルタイプ
	if (modelType == ModelType::kSphere) {
		model_ = wireframeObject3dCommon_->GetModelManager()->FindModel("sphere");
	} else if (modelType == ModelType::kCube) {
		model_ = wireframeObject3dCommon_->GetModelManager()->FindModel("cube");
	}
	//マテリアルの設定
	model_->SetMaterial(material_);
#endif // _DEBUG

	//モデルタイプをリリース時でも使っている風にしている
	(void)modelType;

	//DirectXの基盤部分を受け取る
	directXBase_ = wireframeObject3dCommon_->GetDirectXBase();

	//uv座標
	uvTransform_ = { {1.0f,1.0f},0.0f,{0.0f,0.0f} };
	//DirectXの基盤部分を受け取る
	directXBase_ = wireframeObject3dCommon->GetDirectXBase();
	//SRVマネージャーを受け取る
	srvManager_ = wireframeObject3dCommon->GetSRVManager();
	//座標変換のモード切替用変数
	transform3dMode_ = transform3dMode;
	//トランスフォームのデータ数を決定
	transforms_.resize(instanceCount);
	for (TransformData& transform : transforms_) {
		transform.scale = Vector3::MakeAllOne();
		transform.quaternion = {};
		transform.translate = { 0.0f,0.0f,0.0f };
	}
	//wvpデータも初期化
	wvpData_.resize(instanceCount);
	for (TransformationMatrix& wvp : wvpData_) {
		wvp.world = Matrix4x4::Identity4x4();
		wvp.wvp = Matrix4x4::Identity4x4();
		wvp.worldInverseTranspose = Matrix4x4::Identity4x4();
	}
	//球
	spheres_.resize(instanceCount);
	//AABB
	aabbs_.resize(instanceCount);
	//OBB
	obbs_.resize(instanceCount);

	//wvpリソースの初期化
	CreateTransformationMatrixResource();
	//座標変換行列リソースのストラクチャバッファの生成
	CreateStructuredBufferForWvp();

	//uv座標
	uvTransform_ = { {1.0f,1.0f},0.0f,{0.0f,0.0f} };

	//カメラにデフォルトカメラを代入
	camera_ = camera;
	//カメラをセット
	wireframeObject3dCommon->CreateCameraResource(camera_->GetTranslate());

	//マテリアルの初期化
	material_.color = { 1.0f,1.0f,1.0f,1.0f };
	material_.enableLighting = true;
	material_.uvMatrix = Matrix4x4::Identity4x4();
	material_.shininess = 10.0f;
}


//更新
void WireframeObject3d::Update() {
	//Object3dの共通部分の更新
	wireframeObject3dCommon_->Update();

	if (model_) {
		node_ = model_->GetModelData().rootNode;
		model_->UVTransform(uvTransform_);
	}
	//トランスフォームの更新
	(this->*UpdateTransformTable[static_cast<uint32_t>(transform3dMode_)])();

	//Sphere
	for (Sphere& sphere : spheres_) {
		sphere.center = WireframeObject3d::GetWorldPos(0);
	}

	//AABB
	for (int32_t i = 0; i < static_cast<int32_t>(aabbs_.size()); i++) {
		aabbs_[i].min = WireframeObject3d::GetWorldPos(0) * -transforms_[i].scale;
		aabbs_[i].max = WireframeObject3d::GetWorldPos(0) * transforms_[i].scale;
	}

	//OBB
	for (int32_t i = 0; i < static_cast<int32_t>(obbs_.size()); i++) {
		obbs_[i].center = WireframeObject3d::GetWorldPos(0);
		obbs_[i].quaternion = { transforms_[i].quaternion.x,transforms_[i].quaternion.y,transforms_[i].quaternion.z };
		Rendering::MakeOBBRotateMatrix(obbs_[i].orientations, obbs_[i].quaternion);
		obbs_[i].size = transforms_[i].scale;
	}
}

//描画
void WireframeObject3d::Draw() {
#ifdef _DEBUG
	//3Dオブジェクトの共通部分
	wireframeObject3dCommon_->DrawSetting();

	//PSOの設定
	auto pso = wireframeObject3dCommon_->GetGraphicsPipelineStates()[static_cast<int32_t>(blendMode_)].Get();
	//グラフィックスパイプラインをセットするコマンド
	directXBase_->GetCommandList()->SetPipelineState(pso);

	//座標変換行列SRVの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(srvIndex_));

	//平光源CBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(3, wireframeObject3dCommon_->GetDirectionalLightResource()->GetGPUVirtualAddress());
	//点光源のStructuredBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(5, wireframeObject3dCommon_->GetSRVManager()->GetGPUDescriptorHandle(wireframeObject3dCommon_->GetSrvIndexPoint()));
	//スポットライトのStructuredBufferを設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(6, wireframeObject3dCommon_->GetSRVManager()->GetGPUDescriptorHandle(wireframeObject3dCommon_->GetSrvIndexSpot()));

	//3Dモデルが割り当てられていれば描画
	if (model_) {
		model_->Draw(static_cast<uint32_t>(transforms_.size()));
	}
#endif // _DEBUG
}

//描画
void WireframeObject3d::Draw(int32_t objectCount) {
#ifdef _DEBUG
	//3Dオブジェクトの共通部分
	wireframeObject3dCommon_->DrawSetting();

	//PSOの設定
	auto pso = wireframeObject3dCommon_->GetGraphicsPipelineStates()[static_cast<int32_t>(blendMode_)].Get();
	//グラフィックスパイプラインをセットするコマンド
	directXBase_->GetCommandList()->SetPipelineState(pso);

	//座標変換行列SRVの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(srvIndex_));

	//平光源CBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(3, wireframeObject3dCommon_->GetDirectionalLightResource()->GetGPUVirtualAddress());
	//点光源のStructuredBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(5, wireframeObject3dCommon_->GetSRVManager()->GetGPUDescriptorHandle(wireframeObject3dCommon_->GetSrvIndexPoint()));
	//スポットライトのStructuredBufferを設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(6, wireframeObject3dCommon_->GetSRVManager()->GetGPUDescriptorHandle(wireframeObject3dCommon_->GetSrvIndexSpot()));

	//3Dモデルが割り当てられていれば描画
	if (model_) {
		model_->Draw(static_cast<uint32_t>(objectCount));
	}
#endif // _DEBUG
}

//モデルのセッター
void WireframeObject3d::SetModel(const std::string& name) {
	model_ = wireframeObject3dCommon_->GetModelManager()->FindModel(name);
}

//カメラのセッター
void WireframeObject3d::SetCamera(Camera* camera) {
	camera_ = camera;
	wireframeObject3dCommon_->SetCameraForGPU(camera->GetTranslate());
}

// スケールのセッター
void WireframeObject3d::SetScale(uint32_t index, const Vector3& scale) {
	transforms_[index].scale = scale;
}

//半径のセッター
void WireframeObject3d::SetRadius(uint32_t index, float radius) {
	spheres_[index].radius = radius;
	Vector3 scale = { radius,radius,radius };
	transforms_[index].scale = scale;
}

// 回転のセッター
void WireframeObject3d::SetQuaternion(uint32_t index, const Quaternion& quaternion) {
	transforms_[index].quaternion = quaternion;
}

// 平行移動のセッター
void WireframeObject3d::SetTranslate(uint32_t index, const Vector3& translate) {
	transforms_[index].translate = translate;
}

//トランスフォームのセッター
void WireframeObject3d::SetTransformData(uint32_t index, const TransformData& transform) {
	transforms_[index] = transform;
}

// uvスケールのセッター
void WireframeObject3d::SetUVScale(const Vector2& uvScale) {
	uvTransform_.scale = uvScale;
}

// uv回転のセッター
void WireframeObject3d::SetUVRotate(float uvRotate) {
	uvTransform_.rotate = uvRotate;
}

// uv平行移動のセッター
void WireframeObject3d::SetUVTranslate(const Vector2& uvTranslate) {
	uvTransform_.translate = uvTranslate;
}

//色のセッター
void WireframeObject3d::SetColor(const Vector4& color) {
	if (model_) {
		model_->SetColor(color);
	}
}

//テクスチャの変更
void WireframeObject3d::SetTexture(const std::string& filePath) {
	if (model_) {
		model_->SetTexture(filePath);
	}
}

//ブレンドモードのセッター
void WireframeObject3d::SetBlendMode(const BlendMode& blendMode) {
	blendMode_ = blendMode;
}

// スケールのゲッター
const Vector3& WireframeObject3d::GetScale(uint32_t index) const {
	// TODO: return ステートメントをここに挿入します
	return transforms_[index].scale;
}

// 回転のゲッター
const Quaternion& WireframeObject3d::GetQuaternion(uint32_t index) const {
	// TODO: return ステートメントをここに挿入します
	return transforms_[index].quaternion;
}

// 平行移動のゲッター
const Vector3& WireframeObject3d::GetTranslate(uint32_t index) const {
	// TODO: return ステートメントをここに挿入します
	return transforms_[index].translate;
}

// uvスケールのゲッター
const Vector2& WireframeObject3d::GetUVScale() const {
	// TODO: return ステートメントをここに挿入します
	return uvTransform_.scale;
}

// uv回転のゲッター
const float WireframeObject3d::GetUVRotate() const {
	// TODO: return ステートメントをここに挿入します
	return uvTransform_.rotate;
}

// uv平行移動のゲッター
const Vector2& WireframeObject3d::GetUVTranslate() const {
	// TODO: return ステートメントをここに挿入します
	return uvTransform_.translate;
}

//色のゲッター
const Vector4& WireframeObject3d::GetColor() const {
	// TODO: return ステートメントをここに挿入します
	static const Vector4 defaultColor(0, 0, 0, 0);
	if (model_) {
		return model_->GetColor();
	}
	return defaultColor;
}

//トランスフォームデータのゲッター
const TransformData& WireframeObject3d::GetTransformData(uint32_t index) const {
	// TODO: return ステートメントをここに挿入します
	return transforms_[index];
}

//モデルのゲッター
Model* WireframeObject3d::GetModel() {
	if (model_) {
		return model_;
	}
	return nullptr;
}

//ワールド座標のゲッター
Vector3 WireframeObject3d::GetWorldPos(uint32_t index) {
	return { wvpData_[index].world.m[3][0],wvpData_[index].world.m[3][1],wvpData_[index].world.m[3][2] };
}

//座標変換行列リソースの生成
void WireframeObject3d::CreateTransformationMatrixResource() {
	// 配列サイズで確保
	wvpResource_ = directXBase_->CreateBufferResource(sizeof(TransformationMatrix) * transforms_.size());
	//座標変換行列リソースにデータを書き込むためのアドレスを取得してtransformationMatrixDataに割り当てる
	//書き込むためのアドレス
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpPtr_));
	//単位行列を書き込んでおく
	for (uint32_t i = 0; i < static_cast<uint32_t>(transforms_.size()); i++) {
		wvpPtr_[i].wvp = Matrix4x4::Identity4x4();
		wvpPtr_[i].world = Matrix4x4::Identity4x4();
		wvpPtr_[i].worldInverseTranspose = Matrix4x4::Identity4x4();
	}
}

//座標変換行列リソースのストラクチャバッファの生成
void WireframeObject3d::CreateStructuredBufferForWvp() {
	//ストラクチャバッファを生成
	srvIndex_ = srvManager_->Allocate() + TextureManager::kSRVIndexTop;
	srvManager_->CreateSRVForStructuredBuffer(
		srvIndex_,
		wvpResource_.Get(),
		static_cast<uint32_t>(transforms_.size()),
		sizeof(TransformationMatrix)
	);
}

//座標の更新
void WireframeObject3d::UpdateTransform() {
	for (uint32_t i = 0; i < static_cast<uint32_t>(transforms_.size()); i++) {
		wvpData_[i].world = Rendering::MakeAffineMatrix(transforms_[i]);
		//wvpの書き込み
		if (camera_) {
			const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
			wvpData_[i].wvp = node_.localMatrix * wvpData_[i].world * viewProjectionMatrix;
		} else {
			wvpData_[i].wvp = wvpData_[i].world;
		}
		//ワールド行列を送信
		wvpData_[i].world = node_.localMatrix * wvpData_[i].world;
		//逆行列の転置行列を送信
		wvpData_[i].worldInverseTranspose = wvpData_[i].world.InverseTranspose();

		//ポインタに送信
		wvpPtr_[i] = wvpData_[i];
	}
}

//ビルボード行列での更新
void WireframeObject3d::UpdateTransformBillboard() {
	for (uint32_t i = 0; i < static_cast<uint32_t>(transforms_.size()); i++) {
		//カメラがなかったら
		if (!camera_) {
			wvpData_[i].wvp = wvpData_[i].world;
			return;
		}
		wvpData_[i].world = Rendering::MakeBillboardAffineMatrix(camera_->GetWorldMatrix(), transforms_[i]);
		//wvpの書き込み
		const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
		wvpData_[i].wvp = wvpData_[i].world * viewProjectionMatrix;
		//ワールド行列を送信
		wvpData_[i].world = wvpData_[i].world;
		//逆行列の転置行列を送信
		wvpData_[i].worldInverseTranspose = wvpData_[i].world.InverseTranspose();

		//ポインタに送信
		wvpPtr_[i] = wvpData_[i];
	}
}

// 半径のゲッター
float WireframeObject3d::GetRadius(uint32_t index) const {
	return spheres_[index].radius;
}

//球のゲッター
Sphere WireframeObject3d::GetSphere(uint32_t index) const {
	return spheres_[index];
}

//AABBのゲッター
AABB WireframeObject3d::GetAABB(uint32_t index) const {
	return aabbs_[index];
}

//OBBのゲッター
OBB WireframeObject3d::GetOBB(uint32_t index) const {
	return obbs_[index];
}