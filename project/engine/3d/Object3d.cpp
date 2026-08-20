#include "Object3d.h"
#include "DirectXBase.h"
#include "Camera.h"
#include "ModelManager.h"
#include "MatrixUtility.h"
#include "GameObject.h"
#include "Model.h"
#include "Mesh.h"
#include "Culling.h"
#include "LODBuilder.h"
#include "LODController.h"
#include "Object3dRenderer.h"
#include <algorithm>
#include <cassert>

//メンバ関数テーブルの初期化
void(Object3d::* Object3d::UpdateWorldMatrixTable[])() = {
	&MakeWorldMatrix,
	&MakeBillboardWorldMatrix,
};

//コンストラクタ
Object3d::Object3d(GameObject* gameObject) :Component(gameObject){

}

//デストラクタ
Object3d::~Object3d(){
}

//初期化
void Object3d::Initialize(){
	//基底クラスの初期化
	Component::Initialize();

	//ブレンドモードの初期化
	blendMode_ = BlendMode::kNormal;

	//トランスフォームモード
	worldMatrixType_ = WorldMatrixType::kNormal;

	//ワールド行列の初期化
	worldMatrix_ = Matrix4x4::Identity4x4();
	//Nodeのローカル行列の初期化
	node_.localMatrix = Matrix4x4::Identity4x4();

	//LODビルダーの生成
	lodBuilder_ = std::make_unique<LODBuilder>();
	//LODコントローラの生成
	lodController_ = std::make_unique<LODController>();
}

//複製　
std::unique_ptr<Component> Object3d::Clone(GameObject* gameObject) const{
	std::unique_ptr<Object3d>cloneInstance = std::make_unique<Object3d>(gameObject
	);

	//初期化
	cloneInstance->Initialize();

	//Object3d自信が持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	cloneInstance->SetBlendMode(this->blendMode_);
	cloneInstance->SetTransformMode(this->worldMatrixType_);
	return cloneInstance;
}

//更新
void Object3d::Update(){
	//リンクしているゲームオブジェクトを取得
	GameObject* gameObject = GetOwner();

	//もしリンクしているゲームオブジェクトがNullならば
	if (!gameObject){
		return;
	}

	//もしリンクしているゲームオブジェクトが非Activeなら
	if (!gameObject->IsActive()){
		return;
	}

	//ワールド行列の作成
	//ビルボード作成の場合
	if (worldMatrixType_ == WorldMatrixType::kBilboard){
		//描画カメラがNullだったら
		if (!renderCamera_){
			return;
		}
	}

	(this->*UpdateWorldMatrixTable[static_cast<uint32_t>(worldMatrixType_)])();
}

//モデルの設定
void Object3d::SetModel(Model* model, const std::vector<float>& keepRates){
	//元になるモデルを取得
	baseModel_ = model;

	//倍率を保存
	if (!keepRates.empty()){
		lodKeepRates_ = keepRates;
	} else{
		lodKeepRates_ = { 1.0f };
	}

	//currentLODを0に戻す
	currentLOD_ = 0;

	//モデルが存在する場合
	if (baseModel_){
		//nodeにrootNodeを保存
		node_ = baseModel_->GetModelData().rootNode;

		//マテリアルインスタンスを取得
		materialInstance_ = baseModel_->GetDefaultMaterialInstance();
	} else{
		//モデルがなければリセット
		node_ = {};
		node_.localMatrix = Matrix4x4::Identity4x4();

		materialInstance_.reset();
	}
}

//レンダラーを登録
void Object3d::RegisterToRenderer(Object3dRenderer* renderer){
	assert(renderer);
	assert(lodCount_ > 0);
	assert(kMaxInstanceCount_ > 0);

	//まだ登録されてない事の確認
	assert(renderHandle_ == kInvalidObject3dRenderHandle);

	renderHandle_ = renderer->RegisterObject(lodCount_, kMaxInstanceCount_);

	rendererData_.renderHandle = renderHandle_;
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
	//元のモデルがなければ
	if (!baseModel_){
		return;
	}

	//メッシュのサイズを取得
	const std::vector<std::unique_ptr<Mesh>>& meshes = baseModel_->GetMeshes();

	//メッシュの検索キーとサイズを比較
	if (meshIndex > meshes.size()){
		return;
	}

	//メッシュのNullチェック
	if (!meshes[meshIndex]){
		return;
	}

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
void Object3d::SetBlendMode(BlendMode blendMode){
	blendMode_ = blendMode;
}

//トランスフォームモードの設定
void Object3d::SetTransformMode(WorldMatrixType transformMode){
	worldMatrixType_ = transformMode;
}

//uvスケールの取得
const Vector2& Object3d::GetUVScale(uint32_t index) const{
	return lodUvTransforms_[0][index].scale;
}

//uv回転の取得
const float Object3d::GetUVRotate(uint32_t index) const{
	return lodUvTransforms_[0][index].rotate;
}

//uv平行移動の取得
const Vector2& Object3d::GetUVTranslate(uint32_t index) const{
	return lodUvTransforms_[0][index].translate;
}

//UV座標の取得
const Transform2d& Object3d::GetUVTransform(uint32_t index) const{
	return lodUvTransforms_[0][index];
}

//色の取得
const Vector4& Object3d::GetColor(uint32_t index) const{
	static const Vector4 defaultColor(0.0f, 0.0f, 0.0f, 0.0f);
	if (baseModel_){
		return baseModel_->GetColor(index);
	}
	return defaultColor;
}

//ワールド行列の取得
const Matrix4x4& Object3d::GetWorldMatrix()const{
	return worldMatrix_;
}

//ワールド座標の取得
Vector3 Object3d::GetWorldPos(){
	return { worldMatrix_.m[3][0],worldMatrix_.m[3][1],worldMatrix_.m[3][2] };
}

//メッシュのサイズの取得
uint32_t Object3d::GetMeshDataSize(){
	if (!baseModel_){
		return 0;
	}
	return static_cast<uint32_t>(baseModel_->GetModelData().meshDatas.size());
}

//描画データの取得
const Object3dRenderData& Object3d::GetRenderData(){
	return rendererData_;
}

//モデルの取得
Model* Object3d::GetModel(){
	return baseModel_;
}

//モデルの取得
const Model* Object3d::GetModel() const{
	return baseModel_;
}

//LODのポリゴンの割合の取得
const std::vector<float>& Object3d::GetLODKeepRates()const{
	return lodKeepRates_;
}

//モデルが設定されているかどうか
bool Object3d::HasModel()const{
	if (baseModel_){
		return true;
	}
	return false;
}

//ブレンドモードの取得
BlendMode Object3d::GetBlendMode()const{
	return blendMode_;
}

//ワールド行列タイプの取得
WorldMatrixType Object3d::GetWorldMatrixType()const{
	return worldMatrixType_;
}

//マテリアルインスタンスの取得
MaterialInstance* Object3d::GetMaterialInstance(){
	return materialInstance_.get();
}

//マテリアルインスタンスの取得
const MaterialInstance* Object3d::GetMaterialInstance() const{
	return materialInstance_.get();
}

//LOD関係のセットアップ
void Object3d::SetupLOD(){
	//UV座標
	lodUvTransforms_.resize(lodCount_);
	//描画する数
	lodDrawCounts_.resize(lodCount_);
	rendererData_.lodRenderData.matrixCounts.resize(lodCount_);
	//TransformData
	rendererData_.lodRenderData.transformationData.resize(lodCount_);

	for (std::vector<TransformationMatrix>& lodData : rendererData_.lodRenderData.transformationData){
		lodData.resize(kMaxInstanceCount_);
		for (TransformationMatrix& transform : lodData){
			transform.world = Matrix4x4::Identity4x4();
			transform.wvp = Matrix4x4::Identity4x4();
			transform.worldInverseTranspose = Matrix4x4::Identity4x4();
		}
	}
}

//ワールド行列を作成
void Object3d::MakeWorldMatrix(){
	GameObject* gameObject = GetOwner();
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
	worldMatrix_ = worldMatrix;
}

//ビルボード行列の作成
void Object3d::MakeBillboardWorldMatrix(){
	GameObject* gameObject = GetOwner();
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
	worldMatrix_ = worldMatrix;
}

//座標の更新
void Object3d::UpdateWorldTransform(uint32_t lodIndex, uint32_t drawIndex, const Matrix4x4& worldMatrix){
	TransformationMatrix& transformation = rendererData_.lodRenderData.transformationData[lodIndex][drawIndex];

	transformation.world = worldMatrix;

	transformation.worldInverseTranspose = transformation.world.InverseTranspose();

}

//マテリアルを個別化する
void Object3d::EnsureUniqueMaterialInstance(){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}

	//ModelやほかのObject3dと共有中なら個別コピー
	if (!materialInstance_.use_count() > 1){
		materialInstance_ = std::make_shared<MaterialInstance>(*materialInstance_);
	}
}
