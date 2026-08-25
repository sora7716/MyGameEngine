#include "Frustum.h"
#include "Camera.h"
#include "MathUtility.h"
#include <cmath>

//コンストラクタ
debugDraw::Frustum::Frustum(GameObject* gameObject) :BaseShape(gameObject){
}

//デストラクタ
debugDraw::Frustum::~Frustum(){
}

//初期化
void debugDraw::Frustum::Initialize(DirectXBase* directXBase, Camera* camera){
	vertexCount_ = 8;
	indexCount_ = 24;
	targetCamera_ = camera;

	//基底クラスの更新
	BaseShape::Initialize(directXBase, camera);
}

//更新
void debugDraw::Frustum::Update(){
	//トランスフォームに送信
	transform_.eulerAngle = targetCamera_->GetEulerAngle();
	transform_.quaternion = targetCamera_->GetQuaternion();
	transform_.translate = targetCamera_->GetTranslate();

	//基底クラスの更新
	BaseShape::Update();
}

//対象となるカメラの設定
void debugDraw::Frustum::SetTargetCamera(Camera* targetCamera){
	targetCamera_ = targetCamera;
}

//頂点の設定
void debugDraw::Frustum::SettingVertexData(){
	for (uint32_t i = 0; i < 8; i++){
		//w=1.0fを入れて同次座標系に変換
		vertexData_[i].position = targetCamera_->GetFrustum().localCorners[i];
	}

	//texcoordとnormalは同じ
	for (int32_t i = 0; i < vertexCount_; i++){
		vertexData_[i].texcoord = { 0.0f,0.0f };
		vertexData_[i].normal = { 0.0f,0.0f,1.0f };
	}
}

//インデックスの設定
void debugDraw::Frustum::SettingIndexData(){
	int32_t indices[] = {
		//前面
		0,1,
		1,2,
		2,3,
		3,0,

		//背面
		4,5,
		5,6,
		6,7,
		7,4,

		//接続
		0,4,
		1,5,
		2,6,
		3,7
	};

	//作成したインデックスデータを代入前面
	for (int32_t i = 0; i < indexCount_; i++){
		indexData_[i] = indices[i];
	}
}
