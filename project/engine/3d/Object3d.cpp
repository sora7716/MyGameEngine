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
	//LODの描画カウントのリセット
	for (uint32_t& lodDrawCount : lodDrawCounts_){
		lodDrawCount = 0;
	}

	//描画データをリセット
	rendererData_.lodRenderData.drawCounts = lodDrawCounts_;
	rendererData_.blendMode = blendMode_;

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

	//RootNodeを取得
	if (baseModel_){
		node_ = baseModel_->GetModelData().rootNode;
	}

	//ワールド行列の作成
	if (worldMatrixType_ == WorldMatrixType::kNone){
		return;
	}

	//ビルボード作成の場合
	if (worldMatrixType_ == WorldMatrixType::kBilboard){
		//描画カメラがNullだったら
		if (!renderCamera_){
			return;
		}
	}

	(this->*UpdateWorldMatrixTable[static_cast<uint32_t>(worldMatrixType_)])();

	//モデルが未設定なら
	if (!baseModel_){
		return;
	}

	//isVisibleのリセット
	isVisible_ = false;

	//カリングをする
	for (const std::unique_ptr<Mesh>& mesh : baseModel_->GetMeshes()){
		//isVisibleがtrueだった場合
		if (isVisible_){
			break;
		}

		//カリングがNullだった場合
		if (!culling_){
			isVisible_ = true;
			continue;
		}

		//カリングしているか確認
		isVisible_ = culling_->IsVisibleInFrustum(mesh->GetAABB(), worldMatrix_);
	}

	//isVisibleがfalseだった場合
	if (!isVisible_){
		return;
	}

	//ゲームカメラがNullだった場合
	if (!gameCamera_){
		return;
	}

	//LODの選択
	Vector3 cameraWorldPos = gameCamera_->GetWorldPos();
	Vector3 objectWorldPos = GetWorldPos();

	float distance = (objectWorldPos - cameraWorldPos).Length();

	uint32_t lodIndex = lodController_->SelectLOD(distance, currentLOD_);
	currentLOD_ = lodIndex;

	//描画の検索キー
	uint32_t drawIndex = 0;

	//検索キーとLODModelの要素数を比較して検索キーの方が大きければ
	if (lodIndex >= lodBuilder_->LODModelSize()){
		return;
	}

	//LODModelの取得
	Model* lodModel = lodBuilder_->GetLODModel(lodIndex);
	//LODModelがNullか確認
	if (!lodModel){//Nullだったら
		return;
	}

	//描画カウントまたは描画データのTransformationDataがなかった場合
	if (lodDrawCounts_.empty() ||
		rendererData_.lodRenderData.transformationData.empty() ||
		lodDrawCounts_.size() <= lodIndex ||
		rendererData_.lodRenderData.transformationData.size() <= lodIndex){
		return;
	}

	//描画の検索キーをLODごとに取得
	drawIndex = lodDrawCounts_[lodIndex];

	//TransformationData配列の確認
	const std::vector<std::vector<TransformationMatrix>>& transformationData = rendererData_.lodRenderData.transformationData;
	//LODの検索キーと外側の配列を比べて
	if (lodIndex >= transformationData.size()){
		return;
	}
	//描画カウントと内側の配列を比べて
	if (drawIndex >= transformationData[lodIndex].size()){
		return;
	}

	//座標の更新
	UpdateWorldTransform(lodIndex, drawIndex, worldMatrix_);

	//描画カウントを加算
	lodDrawCounts_[lodIndex]++;

	//描画データへ反映
	rendererData_.lodRenderData.drawCounts = lodDrawCounts_;
	rendererData_.blendMode = blendMode_;

	//LODごとのUV座標がなかった場合
	if (lodUvTransforms_.size() <= lodIndex){
		return;
	}
	//モデルが存在したらメッシュごとにUV座標を適応
	for (uint32_t i = 0; i < lodModel->GetMeshes().size(); i++){
		uint32_t materialIndex = lodModel->GetMeshes()[i]->GetMaterialIndex();

		//マテリアルの検索キーがUV座標の配列の要素数を超えたら
		if (materialIndex >= lodUvTransforms_[lodIndex].size()){
			continue;
		}


		lodModel->UVTransform(materialIndex, lodUvTransforms_[lodIndex][materialIndex]);
	}
}

//モデルの設定
void Object3d::SetModel(std::unique_ptr<Model> model, const std::vector<float>& keepRates){
	//元になるモデルを取得
	baseModel_ = std::move(model);

	//LODカウントの初期化
	lodCount_ = static_cast<uint32_t>(keepRates.size());
	//LOD関係のセットアップ
	SetupLOD();
	//LODモデルの生成
	lodBuilder_->CreateLODModel(directXBase_, baseModel_.get(), keepRates);
	//LODの制御の初期化
	lodController_->Initialize(lodBuilder_.get());

	//モデルを保存
	for (const std::unique_ptr<Model>& lodModel : lodBuilder_->GetLODModels()){
		ModelRenderData modelRenderData = lodModel->GetModelRenderData();
		rendererData_.lodRenderData.modelRendererDatas.push_back(modelRenderData);
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
Matrix4x4& Object3d::GetWorldMatrix(){
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
	// TODO: return ステートメントをここに挿入します
	return rendererData_;
}

//LOD関係のセットアップ
void Object3d::SetupLOD(){
	//UV座標
	lodUvTransforms_.resize(lodCount_);
	//描画する数
	lodDrawCounts_.resize(lodCount_);
	rendererData_.lodRenderData.drawCounts.resize(lodCount_);
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