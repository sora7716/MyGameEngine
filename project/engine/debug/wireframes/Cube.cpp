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
}

//更新
void debugDraw::Cube::UpdateShape(){
}

//複製
std::unique_ptr<Component> debugDraw::Cube::Clone(GameObject* gameObject) const{
	std::unique_ptr<Cube>cloneInstance = std::make_unique<Cube>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//基底クラスの内容コピー
	CopyBaseSetting(*cloneInstance);
	return cloneInstance;
}

//頂点データの設定
void debugDraw::Cube::SettingVertexData(){
	//AABB
	AABB localAABB = {
		{-0.5f,-0.5f,-0.5f},
		{0.5f,0.5f,0.5f}
	};
	////前面
	// 左上
	vertices_[0] = { localAABB.min.x,localAABB.min.y,localAABB.min.z,1.0f };
	// 右上
	vertices_[1] = { localAABB.max.x,localAABB.min.y,localAABB.min.z,1.0f };
	// 右下
	vertices_[2] = { localAABB.max.x,localAABB.max.y,localAABB.min.z,1.0f };
	// 左下
	vertices_[3] = { localAABB.min.x,localAABB.max.y,localAABB.min.z,1.0f };

	//背面
	// 左上
	vertices_[4] = { localAABB.min.x,localAABB.min.y,localAABB.max.z,1.0f };
	// 右上
	vertices_[5] = { localAABB.max.x,localAABB.min.y,localAABB.max.z,1.0f };
	// 右下
	vertices_[6] = { localAABB.max.x,localAABB.max.y,localAABB.max.z,1.0f };
	// 左下
	vertices_[7] = { localAABB.min.x,localAABB.max.y,localAABB.max.z,1.0f };
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