#include "ActorData.h"
#include "Object3d.h"
#include "WireframeObject3d.h"

//レンダーオブジェクトのobject3dとhitBoxの生成
RenderObject& RenderObject::Create() {
	//Object3dの生成
	object3d = std::make_unique<Object3d>();
	//hitBoxの生成
	hitBox = std::make_unique<WireframeObject3d>();

	return *this;
}

//レンダーオブジェクトのmaterialの初期化
RenderObject& RenderObject::InitializeMaterial() {
	//色を設定
	material.color = Vector4::MakeWhiteColor();
	//ライティングを無し
	material.enableLighting = true;
	//光沢度
	material.shininess = 10.0f;
	//uvMatrixの初期化
	material.uvMatrix = Matrix4x4::Identity4x4();

	return *this;
}

//レンダーオブジェクトを作成
RenderObject&& RenderObject::Build() {
	return std::move(*this);
}