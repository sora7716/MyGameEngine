#include "Object3d.h"
#include "Object3dCommon.h"
#include "DirectXBase.h"
#include "Camera.h"
#include "ModelManager.h"
#include "MatrixUtility.h"
#include "GameObject.h"
#include "Model.h"
#include "Mesh.h"
#include "SRVManager.h"
#include "TextureManager.h"
#include "Culling.h"
#include "LODBuilder.h"
#include "LODController.h"
#include <algorithm>
#include <cassert>
//初期化
void Object3dInstance::Initialize(GameObject* targetGameObject){
	this->gameObject = targetGameObject;
	isEnabled = true;
	currentLOD = 0;
}

//メンバ関数テーブルの初期化
void(Object3d::* Object3d::UpdateWorldMatrixTable[])(uint32_t index) = {
	&MakeWorldMatrix,
	&MakeBillboardWorldMatrix,
};

//インスタンスの設定
std::unique_ptr<Object3d> Object3d::Create(Object3dCommon* object3dCommon, Camera* renderCamera, uint32_t maxInstanceCount, Transform3dMode transform3dMode){
	std::unique_ptr<Object3d>instance = std::make_unique<Object3d>();
	instance->Initialize(object3dCommon, renderCamera, maxInstanceCount, transform3dMode);
	return std::move(instance);
}

//コンストラクタ
Object3d::Object3d(){
}

//デストラクタ
Object3d::~Object3d(){
}

//初期化
void Object3d::Initialize(Object3dCommon* object3dCommon, Camera* renderCamera, uint32_t maxInstanceCount, Transform3dMode transform3dMode){
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
	//ワールド行列のサイズを確保(要素数は増やさない)
	worldMatrixes_.reserve(maxInstanceCount_);

	//LOD関係のセットアップ
	SetupLOD();
	//LODビルダーの生成
	lodBuilder_ = std::make_unique<LODBuilder>();
	//LODコントローラの生成
	lodController_ = std::make_unique<LODController>();

	//カメラにデフォルトカメラを代入
	SetRenderCamera(renderCamera);

	//マテリアルの初期化
	material_.color = { 1.0f,1.0f,1.0f,1.0f };
	material_.enableLighting = true;
	material_.uvMatrix = Matrix4x4::Identity4x4();
	material_.shininess = 10.0f;
}

//更新
void Object3d::Update(){
	//Object3dの共通部分の更新
	object3dCommon_->Update();

	//描画データをまとめる
	rendererData_.lodRenderData.drawCounts = lodDrawCounts_;

	//LOD語との描画数をリセット
	for (uint32_t& lodDrawCount : lodDrawCounts_){
		lodDrawCount = 0;
	}

	//RootNodeはmodelを基準にする
	if (baseModel_){
		node_ = baseModel_->GetModelData().rootNode;
	}

	for (uint32_t instanceIndex = 0; instanceIndex < instanceData_.size(); instanceIndex++){
		GameObject* gameObject = instanceData_[instanceIndex].gameObject;
		//ゲームオブジェクトが存在してない場合
		if (!gameObject){
			continue;
		}

		//生存フラグが立ってなければ
		if (!gameObject->IsActive()){
			continue;
		}

		//ワールド行列の作成
		(this->*UpdateWorldMatrixTable[static_cast<uint32_t>(transform3dMode_)])(instanceIndex);;

		//モデルが存在してなかったら
		if (!baseModel_){
			continue;
		}

		//表示状態の更新
		for (const std::unique_ptr<Mesh>& mesh : baseModel_->GetMeshes()){
			instanceData_[instanceIndex].isEnabled = culling_->IsVisibleInFrustum(mesh->GetAABB(), worldMatrixes_[instanceIndex]);
		}

		//表示しなかったら
		if (!instanceData_[instanceIndex].isEnabled){
			continue;
		}

		//LODの計算
		Vector3 cameraWorldPos = gameCamera_->GetWorldPos();
		Vector3 objectWorldPos = GetWorldPos(instanceIndex);

		float distance = (objectWorldPos - cameraWorldPos).Length();

		uint32_t lodIndex = lodController_->SelectLOD(distance, instanceData_[instanceIndex].currentLOD);
		lodIndices_[instanceIndex] = lodIndex;
		instanceData_[instanceIndex].currentLOD = lodIndex;
		//lodIndex番目がlodModelsに無かったら
		if (!lodBuilder_->GetLODModel(lodIndex)){
			continue;
		}

		//LODごとのWVP配列に詰める
		uint32_t drawIndex = lodDrawCounts_[lodIndex];
		//検索キーがデータのサイズより大きかった場合
		if (drawIndex >= lodWvpData_[lodIndex].size()){
			continue;
		}

		//座標の更新
		UpdateWorldTransform(lodIndex, drawIndex, worldMatrixes_[instanceIndex]);

		//描画カウントを加算
		lodDrawCounts_[lodIndex]++;

		//モデルが存在したらメッシュごとにUV座標を適応
		if (lodBuilder_->GetLODModel(lodIndex)){
			for (uint32_t j = 0; j < lodBuilder_->GetLODModel(lodIndex)->GetMeshes().size(); j++){
				uint32_t materialIndex = lodBuilder_->GetLODModel(lodIndex)->GetMeshes()[j]->GetMaterialIndex();

				//マテリアルの検索キーがUV座標の配列の要素数を超えたら
				if (materialIndex >= lodUvTransforms_[lodIndex].size()){
					continue;
				}


				lodBuilder_->GetLODModel(lodIndex)->UVTransform(materialIndex, lodUvTransforms_[lodIndex][materialIndex]);
			}
		}
	}

}

//モデルの設定
void Object3d::SetModel(const std::string& modelName, const std::vector<float>& keepRates){
	//元になるモデルを取得
	baseModel_ = object3dCommon_->GetModelManager()->FindModel(modelName);

	//LODカウントの初期化
	lodCount_ = static_cast<uint32_t>(keepRates.size());
	//LOD関係のセットアップ
	SetupLOD();
	//LODモデルの生成
	lodBuilder_->CreateLODModel(baseModel_.get(), keepRates);
	//LODの制御の初期化
	lodController_->Initialize(lodBuilder_.get());

	//モデルを保存
	for (const std::unique_ptr<Model>& model : lodBuilder_->GetLODModels()){
		ModelRenderData modelRenderData = model->GetModelRenderData();
		rendererData_.lodRenderData.modelRendererData.push_back(modelRenderData);
	}
}

//インスタンスの追加
uint32_t Object3d::AddInstance(GameObject* gameObject){
	//ゲームオブジェクトがNullじゃないか
	assert(gameObject);
	//インスタンスの最大数を超えてないか
	assert(instanceData_.size() < maxInstanceCount_);

	Object3dInstance instance;
	instance.Initialize(gameObject);

	instanceData_.push_back(instance);

	//ワールド行列の要素数を設定
	worldMatrixes_.resize(instanceData_.size());

	//lodIndexをまとめる配列の要素数を設定
	lodIndices_.resize(instanceData_.size());

	return static_cast<uint32_t>(instanceData_.size() - 1);
}

//カメラの設定
void Object3d::SetGameCamera(Camera* camera){
	gameCamera_ = camera;
	//カリングの初期化
	culling_ = Culling::Create(gameCamera_);
}

//描画に使用するカメラの設定
void Object3d::SetRenderCamera(Camera* camera){
	renderCamera_ = camera;
	rendererData_.renderCamera = renderCamera_;
}

//LODの切り替え距離
void Object3d::SetLODDistances(const std::vector<float>& lodDistances){
	lodController_->SetLODDistances(lodDistances);
}

//ヒステリシス幅の設定
void Object3d::SetHysteresis(float hysteresis){
	lodController_->SetHysteresis(hysteresis);
}

// uvスケールの設定
void Object3d::SetUVScale(uint32_t index, const Vector2& uvScale){
	for (std::vector<Transform2d>& uvTransforms : lodUvTransforms_){
		uvTransforms[index].scale = uvScale;
	}
}

// uv回転の設定
void Object3d::SetUVRotate(uint32_t index, float uvRotate){
	for (std::vector<Transform2d>& uvTransforms : lodUvTransforms_){
		uvTransforms[index].rotate = uvRotate;
	}
}

// uv平行移動の設定
void Object3d::SetUVTranslate(uint32_t index, const Vector2& uvTranslate){
	for (std::vector<Transform2d>& uvTransforms : lodUvTransforms_){
		uvTransforms[index].translate = uvTranslate;
	}
}

//色の設定
void Object3d::SetColor(uint32_t materialIndex, const Vector4& color){
	//元モデルにも適応
	baseModel_->SetColor(materialIndex, color);
	//LODモデルにも適応
	lodBuilder_->SetColor(materialIndex, color);
}

//親の設定
void Object3d::SetParent(const WorldTransform* parent){
	(void)parent;
	//worldTransform_->SetParent(parent);
}

//テクスチャの変更
void Object3d::SetTexture(uint32_t meshIndex, const std::string& imageFileName){
	uint32_t materialIndex = baseModel_->GetMeshes()[meshIndex]->GetMaterialIndex();
	//元モデルにも適応
	baseModel_->SetTexture(materialIndex, imageFileName);
	//LODモデルにも適応
	lodBuilder_->SetTexture(materialIndex, imageFileName);
}
//環境マップの変更
void Object3d::SetEnvironmentMap(uint32_t meshIndex, const std::string& environmentMapFileName){
	uint32_t materialIndex = baseModel_->GetMeshes()[meshIndex]->GetMaterialIndex();
	//元モデルにも適応
	baseModel_->SetEnvironmentMap(materialIndex, environmentMapFileName);
	//LODモデルにも適応
	lodBuilder_->SetEnvironmentMap(materialIndex, environmentMapFileName);
}

//ライティングフラグの設定
void Object3d::SetIsLighting(uint32_t meshIndex, bool isLighting){
	uint32_t materialIndex = baseModel_->GetMeshes()[meshIndex]->GetMaterialIndex();
	//元モデルにも適応
	baseModel_->SetIsLighting(materialIndex, isLighting);
	//LODモデルにも適応
	lodBuilder_->SetIsLighting(materialIndex, isLighting);
}

//輝度の設定
void Object3d::SetShininess(uint32_t meshIndex, float shininess){
	uint32_t materialIndex = baseModel_->GetMeshes()[meshIndex]->GetMaterialIndex();
	//元モデルにも適応
	baseModel_->SetShininess(materialIndex, shininess);
	//LODモデルにも適応
	lodBuilder_->SetShininess(materialIndex, shininess);
}

//環境マップの映り込み度を調整
void Object3d::SetEnvironmentCoefficient(uint32_t meshIndex, float& environmentCoefficient){
	//環境マップの映り込み度を0~1にクランプ
	environmentCoefficient = std::clamp(environmentCoefficient, 0.0f, 1.0f);
	//マテリアルインデックス
	uint32_t materialIndex = baseModel_->GetMeshes()[meshIndex]->GetMaterialIndex();
	//元モデルにも適応
	baseModel_->SetEnvironmentCoefficient(materialIndex, environmentCoefficient);
	//LODモデルにも適応
	lodBuilder_->SetEnvironmentCoefficient(materialIndex, environmentCoefficient);
}

//UV座標の設定
void Object3d::SetUVTransform(uint32_t index, const Transform2d& uvTransform){
	for (std::vector<Transform2d>& uvTransforms : lodUvTransforms_){
		uvTransforms[index] = uvTransform;
	}
}

//ブレンドモードの設定
void Object3d::SetBlendMode(const BlendMode& blendMode){
	blendMode_ = blendMode;
}

//uvスケールの取得
const Vector2& Object3d::GetUVScale(uint32_t index) const{
	// TODO: return ステートメントをここに挿入します
	return lodUvTransforms_[0][index].scale;
}

//uv回転の取得
const float Object3d::GetUVRotate(uint32_t index) const{
	// TODO: return ステートメントをここに挿入します
	return lodUvTransforms_[0][index].rotate;
}

//uv平行移動の取得
const Vector2& Object3d::GetUVTranslate(uint32_t index) const{
	// TODO: return ステートメントをここに挿入します
	return lodUvTransforms_[0][index].translate;
}

//UV座標の取得
const Transform2d& Object3d::GetUVTransform(uint32_t index) const{
	// TODO: return ステートメントをここに挿入します
	return lodUvTransforms_[0][index];
}

//色の取得
const Vector4& Object3d::GetColor(uint32_t index) const{
	// TODO: return ステートメントをここに挿入します
	static const Vector4 defaultColor(0.0f, 0.0f, 0.0f, 0.0f);
	if (baseModel_){
		return baseModel_->GetColor(index);
	}
	return defaultColor;
}

//ワールドマトリックスの取得
Matrix4x4& Object3d::GetWorldMatrix(uint32_t instanceIndex){
	return worldMatrixes_[instanceIndex];
}

//ワールド座標の取得
Vector3 Object3d::GetWorldPos(uint32_t instanceIndex){
	return { worldMatrixes_[instanceIndex].m[3][0],worldMatrixes_[instanceIndex].m[3][1],worldMatrixes_[instanceIndex].m[3][2] };
}

//メッシュのサイズの取得
uint32_t Object3d::GetMeshSize(){
	return static_cast<uint32_t>(baseModel_->GetModelData().meshDatas.size());
}

//描画データの取得
const Object3dRenderData& Object3d::GetRenderData(){
	// TODO: return ステートメントをここに挿入します
	return rendererData_;
}

//LOD関係のセットアップ
void Object3d::SetupLOD(){
	//UV座標
	lodUvTransforms_.resize(lodCount_);
	//LODWvpデータ
	lodWvpData_.resize(lodCount_);
	//ワールドビュープロジェクションのリソース
	lodWvpResources_.resize(lodCount_);
	//ワールドビュープロジェクションのポインタ
	lodWvpPtrs_.resize(lodCount_);
	lodSrvIndices_.resize(lodCount_);
	lodDrawCounts_.resize(lodCount_);
	for (uint32_t lod = 0; lod < lodCount_; lod++){
		//wvpのデータ数を決定
		lodWvpData_[lod].resize(maxInstanceCount_);
		for (TransformationMatrix& wvp : lodWvpData_[lod]){
			wvp.world = Matrix4x4::Identity4x4();
			wvp.wvp = Matrix4x4::Identity4x4();
			wvp.worldInverseTranspose = Matrix4x4::Identity4x4();
		}
	}
	//wvpリソースの初期化
	CreateTransformationMatrixResource();
	//座標変換行列リソースのストラクチャバッファの生成
	CreateStructuredBufferForWvp();
	//SRVの保存
	rendererData_.lodRenderData.srvIndices = lodSrvIndices_;
}

//座標変換行列リソースの生成
void Object3d::CreateTransformationMatrixResource(){
	for (uint32_t lod = 0; lod < lodCount_; lod++){
		//インスタンスの最大数で確保
		lodWvpData_[lod].resize(maxInstanceCount_);

		// 配列サイズで確保
		lodWvpResources_[lod] = directXBase_->CreateBufferResource(sizeof(TransformationMatrix) * maxInstanceCount_);
		//座標変換行列リソースにデータを書き込むためのアドレスを取得してtransformationMatrixDataに割り当てる
		//書き込むためのアドレス
		lodWvpResources_[lod]->Map(0, nullptr, reinterpret_cast<void**>(&lodWvpPtrs_[lod]));
		//単位行列を書き込んでおく
		for (uint32_t i = 0; i < static_cast<uint32_t>(maxInstanceCount_); i++){
			lodWvpPtrs_[lod][i].wvp = Matrix4x4::Identity4x4();
			lodWvpPtrs_[lod][i].world = Matrix4x4::Identity4x4();
			lodWvpPtrs_[lod][i].worldInverseTranspose = Matrix4x4::Identity4x4();
		}
	}
}

//座標変換行列リソースのストラクチャバッファの生成
void Object3d::CreateStructuredBufferForWvp(){
	for (uint32_t lod = 0; lod < lodCount_; lod++){
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
void Object3d::MakeWorldMatrix(uint32_t instanceIndex){
	GameObject* gameObject = instanceData_[instanceIndex].gameObject;
	//ゲームオブジェクトがNullじゃないか
	assert(gameObject);
	Matrix4x4 worldMatrix = Matrix4x4::Identity4x4();

	//このオブジェクト本来のワールド行列を求める
	worldMatrix = matrixUtility::MakeAffineMatrix(gameObject->GetTransform());

	if (parent_){
		worldMatrix = worldMatrix * parent_->GetWorldMatrix();
	}

	worldMatrix = node_.localMatrix * worldMatrix;

	//ワールド行列の配列を上書き
	worldMatrixes_[instanceIndex] = worldMatrix;
}

//ビルボード行列の作成
void Object3d::MakeBillboardWorldMatrix(uint32_t instanceIndex){
	GameObject* gameObject = instanceData_[instanceIndex].gameObject;
	//ゲームオブジェクトがNullじゃないか
	assert(gameObject);
	Matrix4x4 worldMatrix = Matrix4x4::Identity4x4();

	//このオブジェクト本来のワールド行列を求める
	worldMatrix = matrixUtility::MakeBillboardAffineMatrix(renderCamera_->GetWorldMatrix(), gameObject->GetTransform());

	if (parent_){
		worldMatrix = worldMatrix * parent_->GetWorldMatrix();
	}

	worldMatrix = node_.localMatrix * worldMatrix;

	//ワールド行列の配列を上書き
	worldMatrixes_[instanceIndex] = worldMatrix;
}

//座標の更新
void Object3d::UpdateWorldTransform(uint32_t lodIndex, uint32_t drawIndex, const Matrix4x4& worldMatrix){
	lodWvpData_[lodIndex][drawIndex].world = worldMatrix;

	lodWvpData_[lodIndex][drawIndex].worldInverseTranspose = lodWvpData_[lodIndex][drawIndex].world.InverseTranspose();

	lodWvpPtrs_[lodIndex][drawIndex] = lodWvpData_[lodIndex][drawIndex];
}