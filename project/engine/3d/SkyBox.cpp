#include "SkyBox.h"
#include "DirectXBase.h"
#include "MatrixUtility.h"
#include "Camera.h"
#include "GameObject.h"
#include <cassert>

//コンストラクタ
SkyBox::SkyBox(GameObject* gameObject) :Component(gameObject){
}

//デストラクタ
SkyBox::~SkyBox(){
}

//初期化
void SkyBox::Initialize(){
	//基底クラスの初期化
	Component::Initialize();

	//スカイボックス固有のデータの初期化
	imageFileName_ = "engine/resources/textures/skybox_cube.dds";
	blendMode_ = BlendMode::kNone;

}

//初期化
void SkyBox::Initialize(DirectXBase* directXBase, const std::string& imageFileName){
	//DirectXの基盤の記録
	assert(directXBase);
	directXBase_ = directXBase;

	//スプライトファイルパスを記録
	imageFileName_ = "engine/resources/textures/" + imageFileName;
}

//更新
void SkyBox::Update(){
	GameObject* gameObject = GetOwner();

	//ワールド座標の更新
	UpdateTransform();

	//描画に必要なデータのセットアップ
	SetupRenderData();
}

//描画に必要なデータのセットアップ
void SkyBox::SetupRenderData(){
	GameObject* gameObject = GetOwner();

	//描画データをまとめる
	renderData_.blendMode = blendMode_;
	renderData_.imageFileName = imageFileName_;
	renderData_.material = material_;
	renderData_.worldMatrix = worldMatrix_;
	if (gameObject){
		renderData_.isActive = gameObject->IsActive();
	} else{
		renderData_.isActive = false;
	}
}

//複製
std::unique_ptr<Component> SkyBox::Clone(GameObject* gameObject) const{
	std::unique_ptr<SkyBox>cloneInstance = std::make_unique<SkyBox>(gameObject
	);

	//初期化
	cloneInstance->Initialize();

	//SkyBoxが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	cloneInstance->imageFileName_ = this->imageFileName_;
	cloneInstance->blendMode_ = this->blendMode_;
	cloneInstance->material_ = this->material_;
	return cloneInstance;
}

//描画データの取得
const SkyBoxRenderData& SkyBox::GetRenderData(){
	return renderData_;
}

//ワールド座標の更新
void SkyBox::UpdateTransform(){
	GameObject* gameObject = GetOwner();
	//ワールド行列の作成
	worldMatrix_ = matrixUtility::MakeAffineMatrix(gameObject->GetTransform());
}