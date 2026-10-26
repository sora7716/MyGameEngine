#include "GizmoVisualizer.h"
#include "GameObject.h"
#include "BaseCollider.h"
#include "BoxCollider.h"
#include "BaseShape.h"
#include "Cube.h"
#include "Sphere.h"
#include "PrimitiveData.h"
using namespace wireframe;

//コンストラクタ
GizmoVisualizer::GizmoVisualizer(){
}

//デストラクタ
GizmoVisualizer::~GizmoVisualizer(){
}

//初期化
void GizmoVisualizer::Initialize(){
}

//更新
void GizmoVisualizer::Update(){
	//描画データのリセット
	renderDatas_.clear();

	//選択されたゲームオブジェクトがNullなら
	if (!selectedGameObject_){
		return;
	}

	//コライダーの配列を取得
	std::vector<BaseCollider*>baseColliders = selectedGameObject_->GetComponents<BaseCollider>();
	//コライダーの配列を探索
	for (BaseCollider* baseCollider : baseColliders){
		//有効状態を確認
		if (!baseCollider->IsEnabled()){
			continue;
		}

		//コライダータイプを取得
		ColliderType colliderType = baseCollider->GetColliderType();

		//各コライダーごとにワイヤーフレームを生成
		switch (colliderType){
		case ColliderType::kBox:{
				//ワイヤーフレームの生成
				std::unique_ptr<BaseShape> cube = Cube::Create<Cube>();

				//Boxコライダーにキャスト
				BoxCollider* boxCollider = static_cast<BoxCollider*>(baseCollider);

				//AABBを取得
				primitiveData::AABB aabb = boxCollider->GetAABB();

				//SRTの設定
				cube->SetLocalScale(aabb.max - aabb.min);
				cube->SetLocalRotate(boxCollider->GetOwner()->GetTransform().rotate);
				cube->SetLocalTranslate((aabb.min + aabb.max) / 2.0f);

				//更新
				cube->Update();

				//描画データを追加
				renderDatas_.push_back(cube->GetRenderData());
				break;
			}
		case ColliderType::kSphere:{
				std::unique_ptr<BaseShape> sphere = Sphere::Create<Sphere>();
				sphere->Update();
				renderDatas_.push_back(sphere->GetRenderData());
				break;
			}
		}
	}
}

//選択されたゲームオブジェクトの設定
void GizmoVisualizer::SetSelectedGameObject(GameObject* selectedGameObject){
	selectedGameObject_ = selectedGameObject;
}

//ワイヤーフレームの描画データの配列の取得
const std::vector<WireframeRenderData>& GizmoVisualizer::GetWireframeRenderDatas() const{
	return renderDatas_;
}
