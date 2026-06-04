#include "Frustum.h"
#include "Camera.h"
#include "algorithms/Math.h"
#include <cmath>

//コンストラクタ
Primitive::Frustum::Frustum() {
}

//デストラクタ
Primitive::Frustum::~Frustum() {
}

//初期化
void Primitive::Frustum::Initialize(DirectXBase* directXBase, Camera* camera) {
	vertexCount_ = 8;
	indexCount_ = 24;
	targetCamera_ = camera;
	
	//基底クラスの更新
	BaseShape::Initialize(directXBase, camera);
}

//更新
void Primitive::Frustum::Update() {
	//トランスフォームに送信
	transform_.eulerAngle = targetCamera_->GetEulerAngle();
	transform_.quaternion = targetCamera_->GetQuaternion();
	transform_.translate = targetCamera_->GetTranslate();
	
	//基底クラスの更新
	BaseShape::Update();

	//視錐台を作成
	CreateFrustumData();
}

//対象となるカメラの設定
void Primitive::Frustum::SetTargetCamera(Camera* targetCamera) {
	targetCamera_ = targetCamera;
}

//視錐台を取得
const PrimitiveData::Frustum& Primitive::Frustum::GetFrustum() const {
	// TODO: return ステートメントをここに挿入します
	return frustum_;
}

//頂点の設定
void Primitive::Frustum::SettingVertexData() {
	float nearZ = targetCamera_->GetNearClip();
	float farZ = targetCamera_->GetFarClip();
	float fovY = targetCamera_->GetFovY();
	float aspect = targetCamera_->GetAspectRation();

	float nearH = std::tan(fovY * 0.5f) * nearZ;
	float nearW = nearH * aspect;

	float farH = std::tan(fovY * 0.5f) * farZ;
	float farW = farH * aspect;

	//near
	vertexData_[0].position = { -nearW,-nearH,nearZ,1.0f };
	vertexData_[1].position = { -nearW,nearH,nearZ,1.0f };
	vertexData_[2].position = { nearW,nearH,nearZ,1.0f };
	vertexData_[3].position = { nearW,-nearH,nearZ,1.0f };

	//far
	vertexData_[4].position = { -farW,-farH,farZ,1.0f };
	vertexData_[5].position = { -farW,farH,farZ,1.0f };
	vertexData_[6].position = { farW,farH,farZ,1.0f };
	vertexData_[7].position = { farW,-farH,farZ,1.0f };

	//texcoordとnormalは同じ
	for (int32_t i = 0; i < vertexCount_; i++) {
		vertexData_[i].texcoord = { 0.0f,0.0f };
		vertexData_[i].normal = { 0.0f,0.0f,1.0f };
	}

	//頂点を代入
	for (uint32_t i = 0; i < frustum_.corners.size(); i++) {
		frustum_.corners[i] = {
			vertexData_[i].position.x,
			vertexData_[i].position.y,
			vertexData_[i].position.z
		};
	}
}

//インデックスの設定
void Primitive::Frustum::SettingIndexData() {
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
	for (int32_t i = 0; i < indexCount_; i++) {
		indexData_[i] = indices[i];
	}
}

//視錐台の作成
void Primitive::Frustum::CreateFrustumData() {
	for (uint32_t i = 0; i < frustum_.corners.size(); i++) {
		Vector4 local = vertexData_[i].position;
		
		Vector4 world = local * worldMatrix_;

		frustum_.corners[i] = {
			world.x,
			world.y,
			world.z
		};
	}

	const auto& c = frustum_.corners;

	frustum_.planes[PrimitiveData::Frustum::kLeft] = Math::MakePlane(c[0], c[1], c[4]);

	frustum_.planes[PrimitiveData::Frustum::kRight] = Math::MakePlane(c[3], c[7], c[2]);

	frustum_.planes[PrimitiveData::Frustum::kTop] = Math::MakePlane(c[1], c[2], c[5]);

	frustum_.planes[PrimitiveData::Frustum::kBottom] = Math::MakePlane(c[0], c[4], c[3]);

	frustum_.planes[PrimitiveData::Frustum::kNear] = Math::MakePlane(c[0], c[2], c[1]);

	frustum_.planes[PrimitiveData::Frustum::kFar] = Math::MakePlane(c[4], c[5], c[6]);
}
