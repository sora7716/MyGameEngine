#include "BaseShape.h"
#include "DirectXBase.h"
#include "MatrixUtility.h"
#include "MathUtility.h"
#include "Logger.h"
#include "GraphicsPipeline.h"
#include "Camera.h"
#include "ImGuiManager.h"
using namespace Microsoft::WRL;
using namespace debugDraw;

#pragma comment(lib,"d3d12.lib")

//コンストラクタ
debugDraw::BaseShape::BaseShape(GameObject* gameObject) :Component(gameObject){
}

//デストラクタ
BaseShape::~BaseShape(){}

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
	color_ = Vector4::MakeWhiteColor();
	//単位行列で初期化
	worldMatrix_ = Matrix4x4::Identity4x4();

	//インデックスの設定
	SettingIndexData();
}

//更新
void BaseShape::Update(){
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
void BaseShape::SetColor(const Vector4& color){
	color_ = color;
}

//ブレンドモードの設定
void debugDraw::BaseShape::SetBlendMode(BlendMode blendMode){
	blendMode_ = blendMode;
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
void BaseShape::UpdateTransform(){
	GameObject* gameObject = GetOwner();
	worldMatrix_ = matrixUtility::MakeAffineMatrix(gameObject->GetTransform());
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