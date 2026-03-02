#include "Object3d.h"
#include "Object3dCommon.h"
#include "DirectXBase.h"
#include "Camera.h"
#include "func/Math.h"
#include "ModelManager.h"
#include "func/Rendering.h"
#include "ImGuiManager.h"
#include "SRVManager.h"
#include "TextureManager.h"
#include <cassert>
//メンバ関数テーブルの初期化
void(Object3d::* Object3d::UpdateTransformTable[])() = {
	&UpdateTransform,
	&UpdateTransformBillboard,
};

//デストラクタ
Object3d::~Object3d() {
	delete worldTransform_;
}

//初期化
void Object3d::Initialize(Object3dCommon* object3dCommon, Camera* camera, uint32_t instanceCount, Transform3dMode transform3dMode) {
	//3Dオブジェクトの共通部分
	object3dCommon_ = object3dCommon;
	//DirectXの基盤部分を受け取る
	directXBase_ = object3dCommon_->GetDirectXBase();
	//SRVマネージャーを受け取る
	srvManager_ = object3dCommon_->GetSRVManager();
	//座標変換のモード切替用変数
	transform3dMode_ = transform3dMode;
	//トランスフォームのデータ数を決定
	transforms_.resize(instanceCount);
	for (TransformData& transform : transforms_) {
		transform.scale = Vector3::MakeAllOne();
		transform.rotate = {};
		transform.translate = {};
	}
	//wvpのデータ数を決定
	wvpData_.resize(instanceCount);
	for (TransformationMatrix& wvp : wvpData_) {
		wvp.world = Matrix4x4::Identity4x4();
		wvp.wvp = Matrix4x4::Identity4x4();
		wvp.worldInverseTranspose = Matrix4x4::Identity4x4();
	}
	//wvpリソースの初期化
	CreateTransformationMatrixResource();
	//座標変換行列リソースのストラクチャバッファの生成
	CreateStructuredBufferForWvp();

	//uv座標
	uvTransform_ = { {1.0f,1.0f},0.0f,{0.0f,0.0f} };

	//カメラにデフォルトカメラを代入
	camera_ = camera;
	//カメラをセット
	object3dCommon_->CreateCameraResource(camera_->GetTranslate());
	object3dCommon_->SetCameraForGPU(camera->GetTranslate());

	//マテリアルの初期化
	material_.color = { 1.0f,1.0f,1.0f,1.0f };
	material_.enableLighting = true;
	material_.uvMatrix = Matrix4x4::Identity4x4();
	material_.shininess = 10.0f;
}


//更新
void Object3d::Update() {
	//Object3dの共通部分の更新
	object3dCommon_->Update();

	if (model_) {
		node_ = model_->GetModelData().rootNode;
		model_->UVTransform(uvTransform_);
	}
	//トランスフォームの更新
	(this->*UpdateTransformTable[static_cast<uint32_t>(transform3dMode_)])();
}

//描画
void Object3d::Draw() {
	//3Dオブジェクトの共通部分
	object3dCommon_->DrawSetting();

	//PSOの設定
	auto pso = object3dCommon_->GetGraphicsPipelineStates()[static_cast<int32_t>(blendMode_)].Get();
	//グラフィックスパイプラインをセットするコマンド
	directXBase_->GetCommandList()->SetPipelineState(pso);

	//座標変換行列SRVの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(srvIndex_));

	//平光源CBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(3, object3dCommon_->GetDirectionalLightResource()->GetGPUVirtualAddress());
	//点光源のStructuredBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(5, object3dCommon_->GetSRVManager()->GetGPUDescriptorHandle(object3dCommon_->GetSrvIndexPoint()));
	//スポットライトのStructuredBufferを設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(6, object3dCommon_->GetSRVManager()->GetGPUDescriptorHandle(object3dCommon_->GetSrvIndexSpot()));

	//3Dモデルが割り当てられていれば描画
	if (model_) {
		model_->Draw(static_cast<uint32_t>(transforms_.size()));
	}
}

void Object3d::Draw(int32_t instanceCount) {
	//3Dオブジェクトの共通部分
	object3dCommon_->DrawSetting();

	//PSOの設定
	auto pso = object3dCommon_->GetGraphicsPipelineStates()[static_cast<int32_t>(blendMode_)].Get();
	//グラフィックスパイプラインをセットするコマンド
	directXBase_->GetCommandList()->SetPipelineState(pso);

	//座標変換行列SRVの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(srvIndex_));

	//平光源CBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(3, object3dCommon_->GetDirectionalLightResource()->GetGPUVirtualAddress());
	//点光源のStructuredBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(5, object3dCommon_->GetSRVManager()->GetGPUDescriptorHandle(object3dCommon_->GetSrvIndexPoint()));
	//スポットライトのStructuredBufferを設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(6, object3dCommon_->GetSRVManager()->GetGPUDescriptorHandle(object3dCommon_->GetSrvIndexSpot()));

	//3Dモデルが割り当てられていれば描画
	if (model_) {
		model_->Draw(instanceCount);
	}
}

//親子付け
void Object3d::Compose(const WorldTransform* parent) {
	worldTransform_->Compose(parent);
}

//親子関係を解除
void Object3d::Decompose() {
	worldTransform_->Decompose();
}

//モデルのセッター
void Object3d::SetModel(const std::string& name) {
	model_ = object3dCommon_->GetModelManager()->FindModel(name);
}

//カメラのセッター
void Object3d::SetCamera(Camera* camera) {
	camera_ = camera;
	object3dCommon_->SetCameraForGPU(camera->GetTranslate());
}

// スケールのセッター
void Object3d::SetScale(uint32_t index, const Vector3& scale) {
	transforms_[index].scale = scale;
}

// 回転のセッター
void Object3d::SetRotate(uint32_t index, const Vector3& rotate) {
	transforms_[index].rotate = rotate;
}

// 平行移動のセッター
void Object3d::SetTranslate(uint32_t index, const Vector3& translate) {
	transforms_[index].translate = translate;
}

//トランスフォームのセッター
void Object3d::SetTransformData(uint32_t index, const TransformData& transform) {
	transforms_[index] = transform;
}

// uvスケールのセッター
void Object3d::SetUVScale(const Vector2& uvScale) {
	uvTransform_.scale = uvScale;
}

// uv回転のセッター
void Object3d::SetUVRotate(float uvRotate) {
	uvTransform_.rotate = uvRotate;
}

// uv平行移動のセッター
void Object3d::SetUVTranslate(const Vector2& uvTranslate) {
	uvTransform_.translate = uvTranslate;
}

//色のセッター
void Object3d::SetColor(const Vector4& color) {
	if (model_) {
		model_->SetColor(color);
	}
}

//親のセッター
void Object3d::SetParent(const WorldTransform* parent) {
	worldTransform_->SetParent(parent);
}

//テクスチャの変更
void Object3d::SetTexture(const std::string& filePath) {
	if (model_) {
		model_->SetTexture(filePath);
	}
}

//ブレンドモードのセッター
void Object3d::SetBlendMode(const BlendMode& blendMode) {
	blendMode_ = blendMode;
}

// スケールのゲッター
const Vector3& Object3d::GetScale(uint32_t index) const {
	// TODO: return ステートメントをここに挿入します
	return transforms_[index].scale;
}

// 回転のゲッター
const Vector3& Object3d::GetRotate(uint32_t index) const {
	// TODO: return ステートメントをここに挿入します
	return transforms_[index].rotate;
}

// 平行移動のゲッター
const Vector3& Object3d::GetTranslate(uint32_t index) const {
	// TODO: return ステートメントをここに挿入します
	return transforms_[index].translate;
}

// uvスケールのゲッター
const Vector2& Object3d::GetUVScale() const {
	// TODO: return ステートメントをここに挿入します
	return uvTransform_.scale;
}

// uv回転のゲッター
const float Object3d::GetUVRotate() const {
	// TODO: return ステートメントをここに挿入します
	return uvTransform_.rotate;
}

// uv平行移動のゲッター
const Vector2& Object3d::GetUVTranslate() const {
	// TODO: return ステートメントをここに挿入します
	return uvTransform_.translate;
}

//色のゲッター
const Vector4& Object3d::GetColor() const {
	// TODO: return ステートメントをここに挿入します
	static const Vector4 defaultColor(0, 0, 0, 0);
	if (model_) {
		return model_->GetColor();
	}
	return defaultColor;
}

//トランスフォームデータのゲッター
const TransformData& Object3d::GetTransformData(uint32_t index) const {
	// TODO: return ステートメントをここに挿入します
	return transforms_[index];
}

//モデルのゲッター
Model* Object3d::GetModel() {
	if (model_) {
		return model_;
	}
	return nullptr;
}

//ワールドマトリックスのゲッター
Matrix4x4& Object3d::GetWorldMatrix(uint32_t index) {
	return wvpData_[index].world;
}

//ワールド座標のゲッター
Vector3 Object3d::GetWorldPos(uint32_t index) {
	return { wvpData_[index].world.m[3][0],wvpData_[index].world.m[3][1],wvpData_[index].world.m[3][2] };
}

//座標変換行列リソースの生成
void Object3d::CreateTransformationMatrixResource() {
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
void Object3d::CreateStructuredBufferForWvp() {
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
void Object3d::UpdateTransform() {
	for (uint32_t i = 0; i < static_cast<uint32_t>(transforms_.size()); i++) {
		wvpData_[i].world = Rendering::MakeAffineMatrix(transforms_[i]);
		//TransformからWorldMatrixを作る
		if (parent_) {
			wvpData_[i].world = wvpData_[i].world * parent_->GetWorldMatrix();
		}
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
void Object3d::UpdateTransformBillboard() {
	for (uint32_t i = 0; i < static_cast<uint32_t>(transforms_.size()); i++) {
		//カメラがなかったら
		if (!camera_) {
			wvpData_[i].wvp = wvpData_[i].world;
			return;
		}
		wvpData_[i].world = Rendering::MakeBillboardAffineMatrix(camera_->GetWorldMatrix(), transforms_[i]);
		//TransformからWorldMatrixを作る
		if (parent_) {
			wvpData_[i].world = wvpData_[i].world * parent_->GetWorldMatrix();
		}
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