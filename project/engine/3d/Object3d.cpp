#include "Object3d.h"
#include "MatrixUtility.h"
#include "GameObject.h"
#include "Model.h"
#include "LODController.h"
#include "MaterialInstance.h"
#include "BaseScene.h"
#include "ModelManager.h"
#include <cassert>

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
	renderTransformMode_ = RenderTransformMode::kNormal;

	//ワールド行列の初期化
	worldMatrix_ = Matrix4x4::Identity4x4();
	//Nodeのローカル行列の初期化
	node_.localMatrix = Matrix4x4::Identity4x4();

	//LODコントローラの生成
	lodController_ = std::make_unique<LODController>();
}

//複製　
std::unique_ptr<Component> Object3d::Clone(GameObject* gameObject) const{
	std::unique_ptr<Object3d>cloneInstance = std::make_unique<Object3d>(gameObject
	);

	//初期化
	cloneInstance->Initialize();

	//Object3dが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	cloneInstance->SetBlendMode(this->blendMode_);
	cloneInstance->SetRenderTransformMode(this->renderTransformMode_);
	return cloneInstance;
}

//更新
void Object3d::Update(){
	//基底クラスの更新
	Component::Update();

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
	MakeWorldMatrix();
}

//カメラとの距離からLODを更新
void Object3d::UpdateLOD(float distance){
	//元モデルまたはLODコントローラがNullの場合
	if (!baseModel_ || !lodController_){
		currentLOD_ = 0;
		return;
	}

	//距離からLODを選択
	currentLOD_ = lodController_->SelectLOD(distance, currentLOD_, baseModel_->GetLODCount());
}

//ワールド行列を作成
Matrix4x4 Object3d::MakeRenderWorldMatrix(const Matrix4x4& cameraWorldMatrix) const{
	//Normalだった場合
	if (renderTransformMode_ == RenderTransformMode::kNormal){
		return worldMatrix_;
	}

	//もしBillboardだった場合
	GameObject* gameObject = GetOwner();
	//ゲームオブジェクトがない場合
	if (!gameObject){
		return worldMatrix_;
	}

	//ビルボードの作成
	Matrix4x4 renderWorldMatrix = matrixUtility::MakeBillboardAffineMatrix(cameraWorldMatrix, gameObject->GetTransform());

	//ノード分を乗算
	renderWorldMatrix = node_.localMatrix * renderWorldMatrix;

	//一時的に作成したワールド行列を返す
	return renderWorldMatrix;
}

//モデルの設定
void Object3d::SetModel(const std::string& modelName){
	//ゲームオブジェクトを取得
	GameObject* gameObject = GetOwner();
	//ゲームオブジェクトが無ければ
	if (!gameObject){
		return;
	}

	//現在接続されているシーンを取得
	BaseScene* currentScene_ = gameObject->GetCurrentScene();
	//現在接続されているシーンがなければ
	if (!currentScene_){
		return;
	}

	//モデルマネージャを取得
	ModelManager* modelManager = currentScene_->GetSceneContext().modelManager;
	//モデルマネージャーがなければ
	if (!modelManager){
		return;
	}

	//元になるモデルを取得
	baseModel_ = modelManager->FindModel(modelName);

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
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetUVScale(index, uvScale);
}

// uv回転の設定
void Object3d::SetUVRotate(uint32_t index, float uvRotate){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetUVRotate(index, uvRotate);
}

// uv平行移動の設定
void Object3d::SetUVTranslate(uint32_t index, const Vector2& uvTranslate){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetUVTranslate(index, uvTranslate);
}

//色の設定
void Object3d::SetColor(uint32_t index, const Vector4& color){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetColor(index, color);
}

//テクスチャの変更
void Object3d::SetTexture(uint32_t index, const std::string& imageFileName){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	//テクスチャを設定
	materialInstance_->SetTexture(index, "engine/resources/textures/" + imageFileName);
}

//環境マップの変更
void Object3d::SetEnvironmentMap(uint32_t index, const std::string& environmentMapFileName){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetEnvironmentMap(index, "engine/resources/textures/" + environmentMapFileName);
}

//ライティングフラグの設定
void Object3d::SetIsLighting(uint32_t index, bool isLighting){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetIsLighting(index, isLighting);
}

//輝度の設定
void Object3d::SetShininess(uint32_t index, float shininess){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetShininess(index, shininess);
}

//環境マップの映り込み度を調整
void Object3d::SetEnvironmentCoefficient(uint32_t index, float& environmentCoefficient){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetEnvironmentCoefficient(index, environmentCoefficient);
}

//UV座標の設定
void Object3d::SetUVTransform(uint32_t index, const RectTransform& uvTransform){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetUVTransform(index, uvTransform);
}

//ブレンドモードの設定
void Object3d::SetBlendMode(BlendMode blendMode){
	blendMode_ = blendMode;
}

//描画時のトランスフォームモードの設定
void Object3d::SetRenderTransformMode(RenderTransformMode transformMode){
	renderTransformMode_ = transformMode;
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

//モデルの取得
Model* Object3d::GetModel(){
	return baseModel_;
}

//モデルの取得
const Model* Object3d::GetModel() const{
	return baseModel_;
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

//描画時のトランスフォームモードの取得
RenderTransformMode Object3d::GetRenderTransformMode()const{
	return renderTransformMode_;
}

//マテリアルインスタンスの取得
MaterialInstance* Object3d::GetMaterialInstance(){
	return materialInstance_.get();
}

//マテリアルインスタンスの取得
const MaterialInstance* Object3d::GetMaterialInstance() const{
	return materialInstance_.get();
}

//現在のLODに対応した描画用モデルを取得
Model* Object3d::GetRenderModel(){
	//元モデルがNullの場合
	if (!baseModel_){
		return nullptr;
	}

	return baseModel_->GetLODModel(currentLOD_);
}

//現在のLOD番号を取得
uint32_t Object3d::GetCurrentLOD() const{
	return currentLOD_;
}

//ワールド行列を作成
void Object3d::MakeWorldMatrix(){
	GameObject* gameObject = GetOwner();
	//ゲームオブジェクトがNullじゃないか
	assert(gameObject);
	Matrix4x4 worldMatrix = Matrix4x4::Identity4x4();

	//このオブジェクト本来のワールド行列を求める
	worldMatrix = matrixUtility::MakeAffineMatrix(gameObject->GetTransform());

	//ノードから、ワールド行列を作成
	worldMatrix = node_.localMatrix * worldMatrix;

	//ワールド行列の配列を上書き
	worldMatrix_ = worldMatrix;
}

//マテリアルを個別化する
void Object3d::EnsureUniqueMaterialInstance(){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}

	//ModelやほかのObject3dと共有中なら個別コピー
	if (materialInstance_.use_count() > 1){
		materialInstance_ = std::make_shared<MaterialInstance>(*materialInstance_);
	}
}
