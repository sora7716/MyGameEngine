#include "Frustum.h"
#include "Camera.h"
#include "GameObject.h"

//コンストラクタ
wireframe::Frustum::Frustum(){
}

//デストラクタ
wireframe::Frustum::~Frustum(){
}

//初期化
void wireframe::Frustum::InitializeShape(){
	vertexCount_ = 8;
	indexCount_ = 24;
}

//更新
void wireframe::Frustum::UpdateShape(){
	if (targetCamera_){
		SetLocalRotate(targetCamera_->GetQuaternion());
		SetLocalTranslate(targetCamera_->GetTranslate());
	}
}

//複製
std::unique_ptr<wireframe::BaseShape> wireframe::Frustum::Clone() const{
	std::unique_ptr<Frustum>cloneInstance = std::make_unique<Frustum>();

	//初期化
	cloneInstance->Initialize();

	//基底クラスの内容コピー
	CopyBaseSetting(*cloneInstance);

	//内容をコピー
	cloneInstance->targetCamera_ = this->targetCamera_;
	return cloneInstance;
}

//対象となるカメラの設定
void wireframe::Frustum::SetTargetCamera(Camera* targetCamera){
	targetCamera_ = targetCamera;
}

//頂点の設定
void wireframe::Frustum::SettingVertexData(){
	for (uint32_t i = 0; i < 8; i++){
		//w=1.0fを入れて同次座標系に変換
		vertices_[i] = targetCamera_->GetFrustum().localCorners[i];
	}
}

//インデックスの設定
void wireframe::Frustum::SettingIndexData(){
	uint32_t indices[] = {
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
	for (uint32_t i = 0; i < indexCount_; i++){
		indices_[i] = indices[i];
	}
}
