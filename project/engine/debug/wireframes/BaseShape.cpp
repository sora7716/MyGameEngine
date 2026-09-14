#include "BaseShape.h"
#include "GameObject.h"
#include "MatrixUtility.h"
using namespace Microsoft::WRL;

#pragma comment(lib,"d3d12.lib")

//コンストラクタ
debugDraw::BaseShape::BaseShape(GameObject* gameObject) :Component(gameObject){
}

//デストラクタ
debugDraw::BaseShape::~BaseShape(){}

//初期化
void debugDraw::BaseShape::Initialize(){
	//基底クラスの初期化
	Component::Initialize();

	//形の初期化
	InitializeShape();

	//要素数を設定
	vertices_.resize(vertexCount_);
	indices_.resize(indexCount_);

	//白色に初期化
	color_ = Vector4::GetWhiteColor();
	//単位行列で初期化
	worldMatrix_ = Matrix4x4::Identity4x4();

	//インデックスの設定
	SettingIndexData();
}

//更新
void debugDraw::BaseShape::Update(){
	//形の更新
	UpdateShape();

	//頂点データの設定
	SettingVertexData();

	//ワールドトランスフォームの更新
	UpdateTransform();

	//描画データのセットアップ
	SetupRenderData();
}

//色の設定
void debugDraw::BaseShape::SetColor(const Vector4& color){
	color_ = color;
}

//ブレンドモードの設定
void debugDraw::BaseShape::SetBlendMode(BlendMode blendMode){
	blendMode_ = blendMode;
}

//ローカルスケールを設定
void debugDraw::BaseShape::SetLocalScale(const Vector3& scale){
	localTransform_.scale = scale;
}

//ローカルの回転を設定
void debugDraw::BaseShape::SetLocalRotate(const Quaternion& rotate){
	localTransform_.quaternion = rotate;
}

//ローカルの平行移動を設定
void debugDraw::BaseShape::SetLocalTranslate(const Vector3& translate){
	localTransform_.translate = translate;
}

//色の取得
const Vector4& debugDraw::BaseShape::GetColor(){
	return color_;
}

//ブレンドモードの取得
BlendMode debugDraw::BaseShape::GetBlendMode() const{
	return blendMode_;
}

//描画データの取得
const DebugDrawRenderData& debugDraw::BaseShape::GetRenderData(){
	return renderData_;
}

//座標の更新
void debugDraw::BaseShape::UpdateTransform(){
	GameObject* gameObject = GetOwner();
	//ローカルのワールド行列
	Matrix4x4 localMatrix = matrixUtility::MakeAffineMatrix(localTransform_);
	//ゲームオブジェクトのワールド行列
	Matrix4x4 ownerMatrix = matrixUtility::MakeAffineMatrix(gameObject->GetTransform());

	//ワールド行列を設定
	worldMatrix_ = localMatrix * ownerMatrix;
}

//描画に必要なデータのセットアップ
void debugDraw::BaseShape::SetupRenderData(){
	renderData_.blendMode = blendMode_;
	renderData_.material = color_;
	renderData_.worldMatrix = worldMatrix_;
	renderData_.indices = indices_;
	renderData_.vertices = vertices_;
}

//複製する際の元となる設定
void debugDraw::BaseShape::CopyBaseSetting(debugDraw::BaseShape& baseShape)const{
	//BaseShapeが持つ設定だけ複製
	baseShape.SetEnabled(IsEnabled());
	baseShape.color_ = color_;
	baseShape.blendMode_ = blendMode_;
}