#include "GizmoVisualizer.h"
#include "GameObject.h"
#include "BaseCollider.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
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

	//コライダーごとに適当なワイヤーフレームを決める
	SelectColliderWireframe();
}

//選択されたゲームオブジェクトの設定
void GizmoVisualizer::SetSelectedGameObject(GameObject* selectedGameObject){
	selectedGameObject_ = selectedGameObject;
}

//ワイヤーフレームの描画データの配列の取得
const std::vector<WireframeRenderData>& GizmoVisualizer::GetWireframeRenderDatas() const{
	return renderDatas_;
}

//コライダーごとに適当なワイヤーフレームを決める
void GizmoVisualizer::SelectColliderWireframe(){
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

				//OBBを取得
				primitiveData::OBB obb = boxCollider->GetOBB();

				//SRTの設定
				cube->SetLocalScale(obb.size);
				cube->SetLocalRotate(boxCollider->GetOwner()->GetTransform().rotate);
				cube->SetLocalTranslate(obb.center);

				//更新
				cube->Update();

				//描画データを追加
				renderDatas_.push_back(cube->GetRenderData());
				break;
			}
		case ColliderType::kSphere:{
				std::unique_ptr<Sphere> sphere = Sphere::Create<Sphere>();

				//Sphereコライダーにキャスト
				SphereCollider* sphereCollider = static_cast<SphereCollider*>(baseCollider);

				//Sphereを取得
				primitiveData::Sphere sphereData = sphereCollider->GetSphere();

				//中心座標と半径を設定
				sphere->SetSphere(sphereData);

				//更新
				sphere->Update();

				//描画データを追加
				renderDatas_.push_back(sphere->GetRenderData());
				break;
			}
		}
	}
}
