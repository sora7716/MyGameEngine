#include "BaseShape.h"
#include "GameObject.h"
#include "MatrixUtility.h"
using namespace Microsoft::WRL;

#pragma comment(lib,"d3d12.lib")

//コンストラクタ
wireframe::BaseShape::BaseShape(){
}

//デストラクタ
wireframe::BaseShape::~BaseShape(){}

//初期化
void wireframe::BaseShape::Initialize(){
	//形の初期化
	InitializeShape();

	//要素数を設定
	vertices_.resize(vertexCount_);
	indices_.resize(indexCount_);

	//緑色に初期化
	color_ = Color::GetGreen();
	//単位行列で初期化
	worldMatrix_ = Matrix4x4::Identity4x4();
	//ブレンドモードを切り替え
	blendMode_ = BlendMode::kNone;

	//インデックスの設定
	SettingIndexData();
}

//更新
void wireframe::BaseShape::Update(){
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
void wireframe::BaseShape::SetColor(const Color& color){
	color_ = color;
}

//ブレンドモードの設定
void wireframe::BaseShape::SetBlendMode(BlendMode blendMode){
	blendMode_ = blendMode;
}

//ローカルスケールを設定
void wireframe::BaseShape::SetLocalScale(const Vector3& scale){
	localTransform_.scale = scale;
}

//ローカルの回転を設定
void wireframe::BaseShape::SetLocalRotate(const Quaternion& rotate){
	localTransform_.rotate = rotate;
}

//ローカルの平行移動を設定
void wireframe::BaseShape::SetLocalTranslate(const Vector3& translate){
	localTransform_.translate = translate;
}

//有効状態を取得
void wireframe::BaseShape::SetEnabled(bool isEnabled){
	isEnabled_ = isEnabled;
}

//色の取得
const Color& wireframe::BaseShape::GetColor(){
	return color_;
}

//ブレンドモードの取得
BlendMode wireframe::BaseShape::GetBlendMode() const{
	return blendMode_;
}

//描画データの取得
const WireframeRenderData& wireframe::BaseShape::GetRenderData(){
	return renderData_;
}

//有効状態を取得
bool wireframe::BaseShape::IsEnabled() const{
	return isEnabled_;
}

//座標の更新
void wireframe::BaseShape::UpdateTransform(){
	//ローカルのワールド行列
	Matrix4x4 localMatrix = matrixUtility::MakeAffineMatrix(localTransform_);
	////ゲームオブジェクトのワールド行列
	//Matrix4x4 ownerMatrix = matrixUtility::MakeAffineMatrix(gameObject->GetTransform());

	//ワールド行列を設定
	worldMatrix_ = localMatrix;
}

//描画に必要なデータのセットアップ
void wireframe::BaseShape::SetupRenderData(){
	renderData_.blendMode = blendMode_;
	renderData_.material = color_;
	renderData_.worldMatrix = worldMatrix_;
	renderData_.indices = indices_;
	renderData_.vertices = vertices_;
}

//複製する際の元となる設定
void wireframe::BaseShape::CopyBaseSetting(wireframe::BaseShape& baseShape)const{
	//BaseShapeが持つ設定だけ複製
	baseShape.SetEnabled(IsEnabled());
	baseShape.color_ = color_;
	baseShape.blendMode_ = blendMode_;
}