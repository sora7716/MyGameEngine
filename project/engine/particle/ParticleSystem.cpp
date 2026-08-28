#define NOMINMAX
#include "ParticleSystem.h"
#include "Model.h"
#include "MaterialInstance.h"
#include "ParticleEmitter.h"
#include "GameObject.h"
#include <string>

//コンストラクタ
ParticleSystem::ParticleSystem(GameObject* gameObject) :Component(gameObject){
}

//デストラクタ
ParticleSystem::~ParticleSystem(){
}

//初期化
void ParticleSystem::Initialize(){
	//基底クラスの初期化
	Component::Initialize();

	//エミッター
	emitter_ = std::make_unique<ParticleEmitter>();
	emitter_->Initialize();
}

//更新
void ParticleSystem::Update(){
	//基底クラスの更新
	Component::Update();

	GameObject* gameObject = GetOwner();
	//エミッターの位置を設定
	emitter_->SetEmitterPosition(gameObject->GetTransform().translate);

	//エミッターの更新
	emitter_->Update();

	//描画に必要なデータのセットアップ
	SetupRenderData();
}

//複製
std::unique_ptr<Component> ParticleSystem::Clone(GameObject* gameObject) const{
	std::unique_ptr<ParticleSystem>cloneInstance = std::make_unique<ParticleSystem>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//Spriteが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	cloneInstance->blendMode_ = this->blendMode_;
	cloneInstance->model_ = this->model_;
	cloneInstance->node_ = this->node_;
	cloneInstance->materialInstance_ = this->materialInstance_;
	return cloneInstance;
}

//ワールド行列を作成
Matrix4x4 ParticleSystem::MakeRenderWorldMatrix(const Matrix4x4& cameraWorldMatrix) const{
	//もしBillboardだった場合
	GameObject* gameObject = GetOwner();
	//ゲームオブジェクトがない場合
	if (!gameObject){
		return  Matrix4x4::Identity4x4();
	}

	//ビルボードの作成
	Matrix4x4 renderWorldMatrix = matrixUtility::MakeBillboardAffineMatrix(cameraWorldMatrix, gameObject->GetTransform());

	//ノード分を乗算
	renderWorldMatrix = node_.localMatrix * renderWorldMatrix;

	//一時的に作成したワールド行列を返す
	return renderWorldMatrix;
}

//ブレンドモードの設定
void ParticleSystem::SetBlendMode(BlendMode blendMode){
	blendMode_ = blendMode;
}

//パーティクルの数の設定
void ParticleSystem::SetParticleCount(uint32_t cont){
	emitter_->SetParticleCount(cont);
}

//発生範囲の設定
void ParticleSystem::SetEmitRange(float range){
	emitter_->SetEmitRange(range);
}

//加速度が起こるフィールドの設定
void ParticleSystem::SetAccelerationField(const AccelerationField& field){
	emitter_->SetAccelerationField(field);
}

//パーティクルの発生感覚[秒]の設定
void ParticleSystem::SetFrequency(float frequency){
	emitter_->SetFrequency(frequency);
}

//モデルの設定
void ParticleSystem::SetModel(Model* model){
	//元になるモデルを取得
	model_ = model;

	//モデルが存在する場合
	if (model_){
		//nodeにrootNodeを保存
		node_ = model_->GetModelData().rootNode;

		//マテリアルインスタンスを取得
		materialInstance_ = model_->GetDefaultMaterialInstance();
	} else{
		//モデルがなければリセット
		node_ = {};
		node_.localMatrix = Matrix4x4::Identity4x4();

		materialInstance_.reset();
	}
}
// uvスケールの設定
void ParticleSystem::SetUVScale(uint32_t index, const Vector2& uvScale){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetUVScale(index, uvScale);
}

// uv回転の設定
void ParticleSystem::SetUVRotate(uint32_t index, float uvRotate){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetUVRotate(index, uvRotate);
}

// uv平行移動の設定
void ParticleSystem::SetUVTranslate(uint32_t index, const Vector2& uvTranslate){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetUVTranslate(index, uvTranslate);
}

//色の設定
void ParticleSystem::SetColor(uint32_t index, const Vector4& color){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetColor(index, color);
}

//テクスチャの変更
void ParticleSystem::SetTexture(uint32_t index, const std::string& imageFileName){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetTexture(index, "engine/resources/textures/" + imageFileName);
}

//UV座標の設定
void ParticleSystem::SetUVTransform(uint32_t index, const RectTransform& uvTransform){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetUVTransform(index, uvTransform);
}

//モデルの取得
Model* ParticleSystem::GetModel(){
	return model_;
}

//描画データの取得
const ParticleRenderData& ParticleSystem::GetRenderData(){
	return renderData_;
}

//モデルを所有しているか
bool ParticleSystem::HasModel() const{
	if (model_){
		return true;
	}
	return false;
}

//マテリアルを個別化する
void ParticleSystem::EnsureUniqueMaterialInstance(){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}

	//ModelやほかのObject3dと共有中なら個別コピー
	if (materialInstance_.use_count() > 1){
		materialInstance_ = std::make_shared<MaterialInstance>(*materialInstance_);
	}
}

//描画に必要なデータのセットアップ
void ParticleSystem::SetupRenderData(){
	renderData_.blendMode = blendMode_;
	renderData_.materialInstance = materialInstance_.get();
	renderData_.model = model_;
	renderData_.numInstance = emitter_->GetNumInstance();
	renderData_.particles = &emitter_->GetParticles();
}
