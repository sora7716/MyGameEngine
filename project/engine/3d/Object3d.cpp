#include "Object3d.h"
#include "Object3dCommon.h"
#include "DirectXBase.h"
#include "Camera.h"
#include "ModelManager.h"
#include "algorithms/Rendering.h"
#include "ImGuiManager.h"
#include "Model.h"
#include "Mesh.h"
#include "SRVManager.h"
#include "TextureManager.h"
#include "algorithms/Collision.h"
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
	//ゲームオブジェクトの数を決定
	gameObjects_.resize(instanceCount);
	//ゲームオブジェクトの初期化
	for (GameObject& gameObject : gameObjects_) {
		gameObject.Initialize();
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
		for (uint32_t i = 0; i < model_->GetMeshes().size(); i++) {
			uint32_t materialIndex = model_->GetMeshes()[i]->GetMaterialIndex();
			model_->UVTransform(materialIndex, uvTransform_[i]);
		}
	}

	//描画するオブジェクト数
	drawCount_ = 0;

	for (int32_t i = 0; i < gameObjects_.size(); i++) {
		//生存フラグが立ってなければ
		if (!gameObjects_[i].isActive) {
			continue;
		}


		//このオブジェクト本来のワールド行列を求める
		Matrix4x4 worldMatrix = Rendering::MakeAffineMatrix(gameObjects_[i].transform);

		if (parent_) {
			worldMatrix = worldMatrix * parent_->GetWorldMatrix();
		}

		worldMatrix = node_.localMatrix * worldMatrix;

		//表示するかのフラグ
		bool isVisible = true;
		if (frustum_ && model_) {
			isVisible = false;
			for (const std::unique_ptr<Mesh>& mesh : model_->GetMeshes()) {
				PrimitiveData::AABB worldAABB = mesh->GetAABB() * worldMatrix;

				if (Collision::IsCollision(*frustum_, worldAABB)) {
					isVisible = true;
					break;
				}
			}
		}

		gameObjects_[i].isEnabled = isVisible;

		if (!isVisible) {
			continue;
		}

		//描画するオブジェクトだけを0~aliveCount-1に詰める
		gameObjects_[drawCount_] = gameObjects_[i];
		//描画するオブジェクトを増やす
		drawCount_++;
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
		model_->Draw(static_cast<uint32_t>(drawCount_));
	}
}

//モデルの設定
void Object3d::SetModel(const std::string& name) {
	model_ = object3dCommon_->GetModelManager()->FindModel(name);
	//uv座標
	uvTransform_.resize(model_->GetModelData().material.size());
	for (uint32_t i = 0; i < uvTransform_.size(); i++) {
		uvTransform_[i] = { {1.0f,1.0f},0.0f,{0.0f,0.0f} };
	}
}

//カメラの設定
void Object3d::SetCamera(Camera* camera) {
	camera_ = camera;
	object3dCommon_->SetCameraForGPU(camera->GetTranslate());
}

// スケールの設定
void Object3d::SetScale(uint32_t index, const Vector3& scale) {
	gameObjects_[index].transform.scale = scale;
}

//クォータニオンの取得
void Object3d::SetQuaternion(uint32_t index, const Quaternion& quaternion) {
	gameObjects_[index].transform.quaternion = quaternion;
}

// 平行移動の設定
void Object3d::SetTranslate(uint32_t index, const Vector3& translate) {
	gameObjects_[index].transform.translate = translate;
}

//ゲームオブジェクトの設定
void Object3d::SetGameObject(uint32_t index, const GameObject& gameObject) {
	gameObjects_[index] = gameObject;
}

// uvスケールの設定
void Object3d::SetUVScale(uint32_t index, const Vector2& uvScale) {
	uvTransform_[index].scale = uvScale;
}

// uv回転の設定
void Object3d::SetUVRotate(uint32_t index, float uvRotate) {
	uvTransform_[index].rotate = uvRotate;
}

// uv平行移動の設定
void Object3d::SetUVTranslate(uint32_t index, const Vector2& uvTranslate) {
	uvTransform_[index].translate = uvTranslate;
}

//色の設定
void Object3d::SetColor(uint32_t index, const Vector4& color) {
	if (model_) {
		model_->SetColor(index, color);
	}
}

//親の設定
void Object3d::SetParent(const WorldTransform* parent) {
	worldTransform_->SetParent(parent);
}

//テクスチャの変更
void Object3d::SetTexture(uint32_t index, const std::string& filePath) {
	if (model_) {
		model_->SetTexture(index, filePath);
	}
}

//UV座標の設定
void Object3d::SetUVTransform(uint32_t index, const Transform2d& uvTransform) {
	uvTransform_[index] = uvTransform;
}

//ブレンドモードの設定
void Object3d::SetBlendMode(const BlendMode& blendMode) {
	blendMode_ = blendMode;
}

//視錐台の設定
void Object3d::SetFrustum(PrimitiveData::Frustum* frustum) {
	frustum_ = frustum;
}

//スケールの取得
const Vector3& Object3d::GetScale(uint32_t index) const {
	// TODO: return ステートメントをここに挿入します
	return gameObjects_[index].transform.scale;
}

//クォータニオンの取得
const Quaternion& Object3d::GetQuaternion(uint32_t index) const {
	// TODO: return ステートメントをここに挿入します
	return gameObjects_[index].transform.quaternion;
}

//平行移動の取得
const Vector3& Object3d::GetTranslate(uint32_t index) const {
	// TODO: return ステートメントをここに挿入します
	return gameObjects_[index].transform.translate;
}

//uvスケールの取得
const Vector2& Object3d::GetUVScale(uint32_t index) const {
	// TODO: return ステートメントをここに挿入します
	return uvTransform_[index].scale;
}

//uv回転の取得
const float Object3d::GetUVRotate(uint32_t index) const {
	// TODO: return ステートメントをここに挿入します
	return uvTransform_[index].rotate;
}

//uv平行移動の取得
const Vector2& Object3d::GetUVTranslate(uint32_t index) const {
	// TODO: return ステートメントをここに挿入します
	return uvTransform_[index].translate;
}

//UV座標の取得
const Transform2d& Object3d::GetUVTransform(uint32_t index) const {
	// TODO: return ステートメントをここに挿入します
	return uvTransform_[index];
}

//色の取得
const Vector4& Object3d::GetColor(uint32_t index) const {
	// TODO: return ステートメントをここに挿入します
	static const Vector4 defaultColor(0.0f, 0.0f, 0.0f, 0.0f);
	if (model_) {
		return model_->GetColor(index);
	}
	return defaultColor;
}

//ゲームオブジェクトの取得
const GameObject& Object3d::GetGameObject(uint32_t index) const {
	// TODO: return ステートメントをここに挿入します
	return gameObjects_[index];
}

//モデルの取得
Model* Object3d::GetModel() {
	if (model_) {
		return model_;
	}
	return nullptr;
}

//ワールドマトリックスの取得
Matrix4x4& Object3d::GetWorldMatrix(uint32_t index) {
	return wvpData_[index].world;
}

//ワールド座標の取得
Vector3 Object3d::GetWorldPos(uint32_t index) {
	return { wvpData_[index].world.m[3][0],wvpData_[index].world.m[3][1],wvpData_[index].world.m[3][2] };
}

//座標変換行列リソースの生成
void Object3d::CreateTransformationMatrixResource() {
	// 配列サイズで確保
	wvpResource_ = directXBase_->CreateBufferResource(sizeof(TransformationMatrix) * gameObjects_.size());
	//座標変換行列リソースにデータを書き込むためのアドレスを取得してtransformationMatrixDataに割り当てる
	//書き込むためのアドレス
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpPtr_));
	//単位行列を書き込んでおく
	for (uint32_t i = 0; i < static_cast<uint32_t>(gameObjects_.size()); i++) {
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
		static_cast<uint32_t>(gameObjects_.size()),
		sizeof(TransformationMatrix)
	);
}

//座標の更新
void Object3d::UpdateTransform() {
	for (uint32_t i = 0; i < static_cast<uint32_t>(gameObjects_.size()); i++) {
		wvpData_[i].world = Rendering::MakeAffineMatrix(gameObjects_[i].transform);
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
	for (uint32_t i = 0; i < static_cast<uint32_t>(gameObjects_.size()); i++) {
		//カメラがなかったら
		if (!camera_) {
			wvpData_[i].wvp = wvpData_[i].world;
			return;
		}
		wvpData_[i].world = Rendering::MakeBillboardAffineMatrix(camera_->GetWorldMatrix(), gameObjects_[i].transform);
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