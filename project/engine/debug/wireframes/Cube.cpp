#include "Cube.h"
#include "MatrixUtility.h"
#include "GameObject.h"
using namespace primitiveData;

//コンストラクタ
debugDraw::Cube::Cube(GameObject* gameObject) :BaseShape(gameObject){
}

//デストラクタ
debugDraw::Cube::~Cube(){}

//初期化
void debugDraw::Cube::InitializeShape(){
	vertexCount_ = 8;
	indexCount_ = 24;
	//OBBを初期化
	obb_.Initialize();
}

//更新
void debugDraw::Cube::UpdateShape(){
	GameObject* gameObject = GetOwner();
	obb_.quaternion = gameObject->GetTransform().quaternion;
	obb_.center = gameObject->GetTransform().translate;
	obb_.size = gameObject->GetTransform().scale.Abs();
	matrixUtility::MakeOBBRotateMatrix(obb_.orientations, obb_.quaternion);
}

//複製
std::unique_ptr<Component> debugDraw::Cube::Clone(GameObject* gameObject) const{
	std::unique_ptr<Cube>cloneInstance = std::make_unique<Cube>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//基底クラスの内容コピー
	CopyBaseSetting(*cloneInstance);

	//内容をコピー
	cloneInstance->obb_ = this->obb_;
	return cloneInstance;
}

//OBBのセッター
void debugDraw::Cube::SetOBB(const OBB& obb){
	obb_ = obb;
}

//OBBのゲッター
OBB debugDraw::Cube::GetOBB(){
	return obb_;
}

//AABBのゲッター
AABB debugDraw::Cube::GetAABB(){
	//AABB
	AABB aabb = {
		{obb_.center - obb_.size / 2.0f},
		{obb_.center + obb_.size / 2.0f},
	};
	return aabb;
}

//頂点データの設定
void debugDraw::Cube::SettingVertexData(){
	//AABB
	AABB aabb = {
		{-obb_.size / 2.0f},
		{obb_.size / 2.0f},
	};
	////前面
	// 左上
	vertices_[0] = { aabb.min.x,aabb.min.y,aabb.min.z,1.0f };
	// 右上
	vertices_[1] = { aabb.max.x,aabb.min.y,aabb.min.z,1.0f };
	// 右下
	vertices_[2] = { aabb.max.x,aabb.max.y,aabb.min.z,1.0f };
	// 左下
	vertices_[3] = { aabb.min.x,aabb.max.y,aabb.min.z,1.0f };

	//背面
	// 左上
	vertices_[4] = { aabb.min.x,aabb.min.y,aabb.max.z,1.0f };
	// 右上
	vertices_[5] = { aabb.max.x,aabb.min.y,aabb.max.z,1.0f };
	// 右下
	vertices_[6] = { aabb.max.x,aabb.max.y,aabb.max.z,1.0f };
	// 左下
	vertices_[7] = { aabb.min.x,aabb.max.y,aabb.max.z,1.0f };
}

//インデックスの設定
void debugDraw::Cube::SettingIndexData(){
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
	for (uint32_t i = 0; i < indexCount_; i++){
		indices_[i] = indices[i];
	}
}