#include "Object3d.h"
#include "Object3dCommon.h"
#include "DirectXBase.h"
#include "Camera.h"
#include "ModelManager.h"
#include "algorithms/Rendering.h"
#include "GameObject.h"
#include "ImGuiManager.h"
#include "Model.h"
#include "Mesh.h"
#include "SRVManager.h"
#include "TextureManager.h"
#include "algorithms/Collision.h"
#include <cassert>
//初期化
void Object3dInstance::Initialize(GameObject* gameObject) {
	this->gameObject = gameObject;
	isEnabled = true;
	currentLOD = 0;
}

//メンバ関数テーブルの初期化
void(Object3d::* Object3d::UpdateWorldMatrixTable[])(uint32_t index) = {
	&MakeWorldMatrix,
	&MakeBillboardWorldMatrix,
};

//コンストラクタ
Object3d::Object3d() {
}

//デストラクタ
Object3d::~Object3d() {
}

//初期化
void Object3d::Initialize(Object3dCommon* object3dCommon, Camera* renderCamera, uint32_t maxInstanceCount, Transform3dMode transform3dMode) {
	//3Dオブジェクトの共通部分
	object3dCommon_ = object3dCommon;
	//DirectXの基盤部分を受け取る
	directXBase_ = object3dCommon_->GetDirectXBase();
	//SRVマネージャーを受け取る
	srvManager_ = object3dCommon_->GetSRVManager();
	//座標変換のモード切替用変数
	transform3dMode_ = transform3dMode;
	//ゲームオブジェクトの数を決定
	maxInstanceCount_ = maxInstanceCount;
	//instanceData_.resize(maxInstanceCount);
	for (uint32_t lod = 0; lod < kLODCount; lod++) {
		//wvpのデータ数を決定
		lodWvpData_[lod].resize(maxInstanceCount);
		for (TransformationMatrix& wvp : lodWvpData_[lod]) {
			wvp.world = Matrix4x4::Identity4x4();
			wvp.wvp = Matrix4x4::Identity4x4();
			wvp.worldInverseTranspose = Matrix4x4::Identity4x4();
		}
	}

	//wvpリソースの初期化
	CreateTransformationMatrixResource();
	//座標変換行列リソースのストラクチャバッファの生成
	CreateStructuredBufferForWvp();

	//カメラにデフォルトカメラを代入
	renderCamera_ = renderCamera;
	//カメラをセット
	object3dCommon_->CreateCameraResource(renderCamera_->GetTranslate());
	object3dCommon_->SetCameraForGPU(renderCamera->GetTranslate());

	//マテリアルの初期化
	material_.color = { 1.0f,1.0f,1.0f,1.0f };
	material_.enableLighting = true;
	material_.uvMatrix = Matrix4x4::Identity4x4();
	material_.shininess = 10.0f;

	lodDistances_ = {
		20.0f,
		50.0f
	};
}

//更新
void Object3d::Update() {
	//Object3dの共通部分の更新
	object3dCommon_->Update();

	//LOD語との描画数をリセット
	for (uint32_t lod = 0; lod < kLODCount; lod++) {
		lodDrawCount_[lod] = 0;
	}

	//RootNodeはLOD0を基準にする
	if (lodModels_[0]) {
		node_ = lodModels_[0]->GetModelData().rootNode;
	}

	for (int32_t i = 0; i < instanceData_.size(); i++) {
		GameObject* gameObject = instanceData_[i].gameObject;
		//ゲームオブジェクトが存在してない場合
		if (!gameObject) {
			continue;
		}

		//生存フラグが立ってなければ
		if (!gameObject->IsActive()) {
			continue;
		}

		//ワールド行列の作成
		(this->*UpdateWorldMatrixTable[static_cast<uint32_t>(transform3dMode_)])(i);;

		//表示状態の更新
		UpdateVisibility(i, worldMatrix_);

		//表示しなかったら
		if (!instanceData_[i].isEnabled) {
			continue;
		}

		//LODの計算
		Vector3 cameraWorldPos = gameCamera_->GetWorldPos();
		Vector3 objectWorldPos = GetWorldPos(i);

		float distance = (objectWorldPos - cameraWorldPos).Length();

		uint32_t lodIndex = SelectLOD(distance, instanceData_[i].currentLOD);
		instanceData_[i].currentLOD = lodIndex;
		//lodIndex番目がlodModelsに無かったら
		if (!lodModels_[lodIndex]) {
			continue;
		}

		//LOD語とのWVP配列に詰める
		uint32_t drawIndex = lodDrawCount_[lodIndex];
		//検索キーがデータのサイズより大きかった場合
		if (drawIndex >= lodWvpData_[lodIndex].size()) {
			continue;
		}

		//座標の更新
		UpdateWorldTransform(lodIndex, drawIndex, worldMatrix_);

		//描画カウントを加算
		lodDrawCount_[lodIndex]++;

		//モデルが存在したらメッシュごとにUV座標を適応
		if (lodModels_[lodIndex]) {
			for (uint32_t i = 0; i < lodModels_[lodIndex]->GetMeshes().size(); i++) {
				uint32_t materialIndex = lodModels_[lodIndex]->GetMeshes()[i]->GetMaterialIndex();

				//マテリアルの検索キーがUV座標の配列の要素数を超えたら
				if (materialIndex >= lodUvTransforms_[lodIndex].size()) {
					continue;
				}


				lodModels_[lodIndex]->UVTransform(materialIndex, lodUvTransforms_[lodIndex][materialIndex]);
			}
		}
	}

}

//描画
void Object3d::Draw() {
	//3Dオブジェクトの共通部分
	object3dCommon_->DrawSetting();

	//PSOの設定
	auto pso = object3dCommon_->GetGraphicsPipelineStates()[static_cast<int32_t>(blendMode_)].Get();
	//グラフィックスパイプラインをセットするコマンド
	directXBase_->GetCommandList()->SetPipelineState(pso);

	//平光源CBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(3, object3dCommon_->GetDirectionalLightResource()->GetGPUVirtualAddress());
	//点光源のStructuredBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(5, object3dCommon_->GetSRVManager()->GetGPUDescriptorHandle(object3dCommon_->GetSrvIndexPoint()));
	//スポットライトのStructuredBufferを設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(6, object3dCommon_->GetSRVManager()->GetGPUDescriptorHandle(object3dCommon_->GetSrvIndexSpot()));

	for (uint32_t lod = 0; lod < kLODCount; lod++) {
		//LODモデルが存在してなかったら
		if (!lodModels_[lod]) {
			continue;
		}

		//LOD描画カウントが0だったら
		if (lodDrawCount_[lod] == 0) {
			continue;
		}

		//LODごとのWVP SRVを設定
		directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(lodSrvIndices_[lod]));

		//ドローコール
		lodModels_[lod]->Draw(lodDrawCount_[lod]);

	}
}

//インスタンスの追加
uint32_t Object3d::AddInstance(GameObject* gameObject) {
	//ゲームオブジェクトがNullじゃないか
	assert(gameObject);
	//インスタンスの最大数を超えてないか
	assert(instanceData_.size() < maxInstanceCount_);

	Object3dInstance instance;
	instance.Initialize(gameObject);

	instanceData_.push_back(instance);

	return static_cast<uint32_t>(instanceData_.size() - 1);
}

//LODモデルの設定
void Object3d::SetLODModel(uint32_t lodIndex, const std::string& modelName) {
	assert(lodIndex < kLODCount);

	Model* model = object3dCommon_->GetModelManager()->FindModel(modelName);

	lodModels_[lodIndex] = model;

	//Uマテリアル数を取得
	const uint32_t materialCount = static_cast<uint32_t>(lodModels_[lodIndex]->GetModelData().material.size());

	//UV座標をマテリアルサイズに合わせる
	lodUvTransforms_[lodIndex].resize(materialCount);

	//UV座標の初期化
	for (Transform2d& uvTransform : lodUvTransforms_[lodIndex]) {
		uvTransform.Initialize();
	}
}

//カメラの設定
void Object3d::SetGameCamera(Camera* gameCamera) {
	gameCamera_ = gameCamera;
}

// uvスケールの設定
void Object3d::SetUVScale(uint32_t index, const Vector2& uvScale) {
	for (std::vector<Transform2d>& uvTransforms : lodUvTransforms_) {
		uvTransforms[index].scale = uvScale;
	}
}

// uv回転の設定
void Object3d::SetUVRotate(uint32_t index, float uvRotate) {
	for (std::vector<Transform2d>& uvTransforms : lodUvTransforms_) {
		uvTransforms[index].rotate = uvRotate;
	}
}

// uv平行移動の設定
void Object3d::SetUVTranslate(uint32_t index, const Vector2& uvTranslate) {
	for (std::vector<Transform2d>& uvTransforms : lodUvTransforms_) {
		uvTransforms[index].translate = uvTranslate;
	}
}

//色の設定
void Object3d::SetColor(uint32_t materialIndex, const Vector4& color) {
	for (Model* model : lodModels_) {
		if (model) {
			model->SetColor(materialIndex, color);
		}
	}
}

//親の設定
void Object3d::SetParent(const WorldTransform* parent) {
	//worldTransform_->SetParent(parent);
}

//テクスチャの変更
void Object3d::SetTexture(uint32_t materialIndex, const std::string& filePath) {
	for (Model* model : lodModels_) {
		if (model) {
			model->SetTexture(materialIndex, filePath);
		}
	}
}

//UV座標の設定
void Object3d::SetUVTransform(uint32_t index, const Transform2d& uvTransform) {
	for (std::vector<Transform2d>& uvTransforms : lodUvTransforms_) {
		uvTransforms[index] = uvTransform;
	}
}

//ブレンドモードの設定
void Object3d::SetBlendMode(const BlendMode& blendMode) {
	blendMode_ = blendMode;
}

//uvスケールの取得
const Vector2& Object3d::GetUVScale(uint32_t index) const {
	// TODO: return ステートメントをここに挿入します
	return lodUvTransforms_[0][index].scale;
}

//uv回転の取得
const float Object3d::GetUVRotate(uint32_t index) const {
	// TODO: return ステートメントをここに挿入します
	return lodUvTransforms_[0][index].rotate;
}

//uv平行移動の取得
const Vector2& Object3d::GetUVTranslate(uint32_t index) const {
	// TODO: return ステートメントをここに挿入します
	return lodUvTransforms_[0][index].translate;
}

//UV座標の取得
const Transform2d& Object3d::GetUVTransform(uint32_t index) const {
	// TODO: return ステートメントをここに挿入します
	return lodUvTransforms_[0][index];
}

//色の取得
const Vector4& Object3d::GetColor(uint32_t index) const {
	// TODO: return ステートメントをここに挿入します
	static const Vector4 defaultColor(0.0f, 0.0f, 0.0f, 0.0f);
	if (lodModels_[0]) {
		return lodModels_[0]->GetColor(index);
	}
	return defaultColor;
}

//モデルの取得
Model* Object3d::GetModel() {
	if (lodModels_[0]) {
		return lodModels_[0];
	}
	return nullptr;
}

//ワールドマトリックスの取得
Matrix4x4& Object3d::GetWorldMatrix(uint32_t index) {
	return worldMatrix_;
}

//ワールド座標の取得
Vector3 Object3d::GetWorldPos(uint32_t index) {
	return { worldMatrix_.m[3][0],worldMatrix_.m[3][1],worldMatrix_.m[3][2] };
	return Vector3{};

}

//座標変換行列リソースの生成
void Object3d::CreateTransformationMatrixResource() {
	for (uint32_t lod = 0; lod < kLODCount; lod++) {
		//インスタンスの最大数で確保
		lodWvpData_[lod].resize(maxInstanceCount_);

		// 配列サイズで確保
		lodWvpResources_[lod] = directXBase_->CreateBufferResource(sizeof(TransformationMatrix) * maxInstanceCount_);
		//座標変換行列リソースにデータを書き込むためのアドレスを取得してtransformationMatrixDataに割り当てる
		//書き込むためのアドレス
		lodWvpResources_[lod]->Map(0, nullptr, reinterpret_cast<void**>(&lodWvpPtrs_[lod]));
		//単位行列を書き込んでおく
		for (uint32_t i = 0; i < static_cast<uint32_t>(maxInstanceCount_); i++) {
			lodWvpPtrs_[lod][i].wvp = Matrix4x4::Identity4x4();
			lodWvpPtrs_[lod][i].world = Matrix4x4::Identity4x4();
			lodWvpPtrs_[lod][i].worldInverseTranspose = Matrix4x4::Identity4x4();
		}
	}
}

//座標変換行列リソースのストラクチャバッファの生成
void Object3d::CreateStructuredBufferForWvp() {
	for (uint32_t lod = 0; lod < kLODCount; lod++) {
		//ストラクチャバッファを生成
		lodSrvIndices_[lod] = srvManager_->Allocate() + TextureManager::kSRVIndexTop;
		srvManager_->CreateSRVForStructuredBuffer(
			lodSrvIndices_[lod],
			lodWvpResources_[lod].Get(),
			static_cast<uint32_t>(maxInstanceCount_),
			sizeof(TransformationMatrix)
		);
	}
}

//ワールド行列を作成
void Object3d::MakeWorldMatrix(uint32_t index) {
	GameObject* gameObject = instanceData_[index].gameObject;
	//ゲームオブジェクトがNullじゃないか
	assert(gameObject);

	//このオブジェクト本来のワールド行列を求める
	worldMatrix_ = Rendering::MakeAffineMatrix(gameObject->GetTransform());

	if (parent_) {
		worldMatrix_ = worldMatrix_ * parent_->GetWorldMatrix();
	}

	worldMatrix_ = node_.localMatrix * worldMatrix_;
}

//ビルボード行列の作成
void Object3d::MakeBillboardWorldMatrix(uint32_t index) {
	GameObject* gameObject = instanceData_[index].gameObject;
	//ゲームオブジェクトがNullじゃないか
	assert(gameObject);

	//このオブジェクト本来のワールド行列を求める
	worldMatrix_ = Rendering::MakeBillboardAffineMatrix(renderCamera_->GetWorldMatrix(), gameObject->GetTransform());

	if (parent_) {
		worldMatrix_ = worldMatrix_ * parent_->GetWorldMatrix();
	}

	worldMatrix_ = node_.localMatrix * worldMatrix_;
}

//座標の更新
void Object3d::UpdateWorldTransform(uint32_t lodIndex, uint32_t drawIndex, const Matrix4x4& worldMatrix) {
	lodWvpData_[lodIndex][drawIndex].world = worldMatrix;

	if (renderCamera_) {
		lodWvpData_[lodIndex][drawIndex].wvp = worldMatrix * renderCamera_->GetViewProjectionMatrix();
	} else {
		lodWvpData_[lodIndex][drawIndex].wvp = worldMatrix;
	}

	lodWvpData_[lodIndex][drawIndex].worldInverseTranspose = lodWvpData_[lodIndex][drawIndex].world.InverseTranspose();

	lodWvpPtrs_[lodIndex][drawIndex] = lodWvpData_[lodIndex][drawIndex];
}

//オブジェクトの表示状態の更新
void Object3d::UpdateVisibility(uint32_t index, const Matrix4x4& worldMatrix) {
	//表示するかのフラグ
	bool isVisible = true;
	if (gameCamera_ && lodModels_[0]) {
		isVisible = false;
		for (const std::unique_ptr<Mesh>& mesh : lodModels_[0]->GetMeshes()) {
			PrimitiveData::AABB worldAABB = mesh->GetAABB() * worldMatrix;

			if (Collision::IsCollision(gameCamera_->GetFrustum(), worldAABB)) {
				isVisible = true;
				break;
			}
		}
	}

	instanceData_[index].isEnabled = isVisible;
}

//距離によってLODモデルの添え字を取得
uint32_t Object3d::SelectLOD(float distance, uint32_t currentLOD) const {
	//距離境界が足りない場合
	if (lodDistances_.size() < kLODCount - 1) {
		return 0;
	}

	//currentLODが範囲外なら戻す
	if (currentLOD >= lodModels_.size()) {
		currentLOD = 0;
	}

	//LODモデル番号
	uint32_t result = currentLOD;
	//ヒステリシス幅
	const float hysteresis = 5.0f;

	//現在のLODをみて
	switch (currentLOD) {
	case 0:
		//LOD0 -> LOD1
		if (distance >= lodDistances_[0] + hysteresis) {
			result = 1;
		}
		break;
	case 1:
		//LOD1 -> LOD0
		if (distance < lodDistances_[0] - hysteresis) {
			result = 0;
		} else if (distance >= lodDistances_[1] + hysteresis) {
			//LOD1 -> LOD2
			result = 2;
		}
		break;
	case 2:
		//LOD2 -> LOD1
		if (distance < lodDistances_[1] - hysteresis) {
			result = 1;
		}
		break;
	}

	//選ばれたLODがなければ
	if (!lodModels_[result]) {
		return 0;
	}

	return result;
}