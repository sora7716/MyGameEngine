#include "Object3d.h"
#include "MatrixUtility.h"
#include "GameObject.h"
#include "Model.h"
#include "LODController.h"
#include "MaterialInstance.h"
#include "BaseScene.h"
#include "ModelManager.h"
#include "Logger.h"
#include <cassert>

//コンストラクタ
Object3d::Object3d(GameObject* gameObject) :Component(gameObject){

}

//デストラクタ
Object3d::~Object3d(){
}

//初期化
void Object3d::Initialize(){
	//基底クラスの初期化
	Component::Initialize();

	//ブレンドモードの初期化
	blendMode_ = BlendMode::kNormal;

	//トランスフォームモード
	renderTransformMode_ = RenderTransformMode::kNormal;

	//親行列の初期化
	parentMatrix_ = Matrix4x4::Identity4x4();
	//ローカル行列の初期化
	localMatrix_ = Matrix4x4::Identity4x4();
	//ワールド行列の初期化
	worldMatrix_ = Matrix4x4::Identity4x4();
	//Nodeのローカル行列の初期化
	node_.localMatrix = Matrix4x4::Identity4x4();

	//LODコントローラの生成
	lodController_ = std::make_unique<LODController>();
}

//複製　
std::unique_ptr<Component> Object3d::Clone(GameObject* gameObject) const{
	std::unique_ptr<Object3d>cloneInstance = std::make_unique<Object3d>(gameObject
	);

	//初期化
	cloneInstance->Initialize();

	//Object3dが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	cloneInstance->SetBlendMode(this->blendMode_);
	cloneInstance->SetRenderTransformMode(this->renderTransformMode_);
	return cloneInstance;
}

//更新
void Object3d::Update(){
	//基底クラスの更新
	Component::Update();

	//リンクしているゲームオブジェクトを取得
	GameObject* gameObject = GetOwner();

	//もしリンクしているゲームオブジェクトがNullならば
	if (!gameObject){
		return;
	}

	//もしリンクしているゲームオブジェクトが非Activeなら
	if (!gameObject->IsActive()){
		return;
	}

	//ワールド行列の作成
	MakeWorldMatrix();
}

//カメラとの距離からLODを更新
void Object3d::UpdateLOD(float distance){
	//元モデルまたはLODコントローラがNullの場合
	if (!baseModel_ || !lodController_){
		currentLOD_ = 0;
		return;
	}

	//距離からLODを選択
	currentLOD_ = lodController_->SelectLOD(distance, currentLOD_, baseModel_->GetLODCount());
}

//親子付けを外す
void Object3d::DetachParent(){
	//そもそも親がなければ
	if (!parentObject3d_){
		return;
	}
	//ゲームオブジェクト取得
	GameObject* gameObject = GetOwner();
	//ゲームオブジェクトがなければ
	if (!gameObject){
		return;
	}

	//Transformを抽出
	Transform& transform = gameObject->GetTransform();
	transform = matrixUtility::DecomposeMatrix(worldMatrix_, transform.scale);

	//親を解除
	parentObject3d_ = nullptr;
	parentNodePath_.clear();
	parentMatrix_ = Matrix4x4::Identity4x4();
}

//親子付け
bool Object3d::AttachTo(Object3d* parent, const std::string& parentNodePath){
	if (!parent){
		Logger::OutputLog("親子付けを行う際の親がnull参照しました");
		return false;
	}

	if (parent == this){
		Logger::OutputLog("親子付けを行う際に自分のポインタを参照しました");
		return false;
	}

	Node* found = parent->FindNode(parentNodePath);
	if (!found){
		Logger::OutputLog("存在しないNodeのPathにアクセスしようとしました");
		return false;
	}

	//メンバ変数に記録
	parentObject3d_ = parent;
	parentNodePath_ = parentNodePath;
	return true;
}

//ワールド行列を作成
Matrix4x4 Object3d::MakeRenderWorldMatrix(const Matrix4x4& cameraWorldMatrix) const{
	//Normalだった場合
	if (renderTransformMode_ == RenderTransformMode::kNormal){
		return worldMatrix_;
	}

	//もしBillboardだった場合
	GameObject* gameObject = GetOwner();
	//ゲームオブジェクトがない場合
	if (!gameObject){
		return worldMatrix_;
	}

	//ビルボードの作成
	Matrix4x4 renderWorldMatrix = matrixUtility::MakeBillboardAffineMatrix(cameraWorldMatrix, gameObject->GetTransform());

	//一時的に作成したワールド行列を返す
	return renderWorldMatrix;
}

//モデルの設定
void Object3d::SetModel(const std::string& modelName){
	//ゲームオブジェクトを取得
	GameObject* gameObject = GetOwner();
	//ゲームオブジェクトが無ければ
	if (!gameObject){
		return;
	}

	//現在接続されているシーンを取得
	BaseScene* currentScene_ = gameObject->GetCurrentScene();
	//現在接続されているシーンがなければ
	if (!currentScene_){
		return;
	}

	//モデルマネージャを取得
	ModelManager* modelManager = currentScene_->GetSceneContext().modelManager;
	//モデルマネージャーがなければ
	if (!modelManager){
		return;
	}

	//元になるモデルを取得
	baseModel_ = modelManager->FindModel(modelName);

	//currentLODを0に戻す
	currentLOD_ = 0;

	//モデルが存在する場合
	if (baseModel_){
		//メッシュのワールド行列のサイズ決定(要素数は確保しない)
		nodeMeshInstance_.reserve(baseModel_->GetMeshes().size());

		//nodeにrootNodeを保存
		node_ = baseModel_->GetModelData().rootNode;

		//マテリアルインスタンスを取得
		materialInstance_ = baseModel_->GetDefaultMaterialInstance();
	} else{
		//モデルがなければリセット
		node_ = {};
		node_.localMatrix = Matrix4x4::Identity4x4();

		materialInstance_.reset();
	}
}

//LODの切り替え距離
void Object3d::SetLODDistances(const std::vector<float>& lodDistances){
	lodController_->SetLODDistances(lodDistances);
}

//ヒステリシス幅の設定
void Object3d::SetHysteresis(float hysteresis){
	lodController_->SetHysteresis(hysteresis);
}

// uvスケールの設定
void Object3d::SetUVScale(uint32_t index, const Vector2& uvScale){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetUVScale(index, uvScale);
}

// uv回転の設定
void Object3d::SetUVRotate(uint32_t index, float uvRotate){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetUVRotate(index, uvRotate);
}

// uv平行移動の設定
void Object3d::SetUVTranslate(uint32_t index, const Vector2& uvTranslate){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetUVTranslate(index, uvTranslate);
}

//色の設定
void Object3d::SetColor(uint32_t index, const Vector4& color){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetColor(index, color);
}

//テクスチャの変更
void Object3d::SetTexture(uint32_t index, const std::string& imageFileName){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	//テクスチャを設定
	materialInstance_->SetTexture(index, "engine/resources/textures/" + imageFileName);
}

//環境マップの変更
void Object3d::SetEnvironmentMap(uint32_t index, const std::string& environmentMapFileName){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetEnvironmentMap(index, "engine/resources/textures/" + environmentMapFileName);
}

//ライティングフラグの設定
void Object3d::SetIsLighting(uint32_t index, bool isLighting){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetIsLighting(index, isLighting);
}

//輝度の設定
void Object3d::SetShininess(uint32_t index, float shininess){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetShininess(index, shininess);
}

//環境マップの映り込み度を調整
void Object3d::SetEnvironmentCoefficient(uint32_t index, float& environmentCoefficient){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetEnvironmentCoefficient(index, environmentCoefficient);
}

//UV座標の設定
void Object3d::SetUVTransform(uint32_t index, const RectTransform& uvTransform){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetUVTransform(index, uvTransform);
}

//ブレンドモードの設定
void Object3d::SetBlendMode(BlendMode blendMode){
	blendMode_ = blendMode;
}

//描画時のトランスフォームモードの設定
void Object3d::SetRenderTransformMode(RenderTransformMode transformMode){
	renderTransformMode_ = transformMode;
}

//ノードのローカルトランスフォームの設定
bool Object3d::SetNodeLocalTransform(const std::string& path, const Transform& localTransform){
	//ノードがあるか探索
	Node* found = FindNode(path);

	//ノードが見つからなかった場合
	if (!found){
		return false;
	}

	//ローカルトランスフォームを設定
	found->localTransform = localTransform;
	return true;
}

//ワールド行列の取得
const Matrix4x4& Object3d::GetWorldMatrix()const{
	return worldMatrix_;
}

//ワールド座標の取得
Vector3 Object3d::GetWorldPos(){
	return { worldMatrix_.m[3][0],worldMatrix_.m[3][1],worldMatrix_.m[3][2] };
}

//メッシュのサイズの取得
uint32_t Object3d::GetMeshDataSize(){
	if (!baseModel_){
		return 0;
	}
	return static_cast<uint32_t>(baseModel_->GetModelData().meshDatas.size());
}

//モデルの取得
Model* Object3d::GetModel(){
	return baseModel_;
}

//モデルの取得
const Model* Object3d::GetModel() const{
	return baseModel_;
}

//モデルが設定されているかどうか
bool Object3d::HasModel()const{
	if (baseModel_){
		return true;
	}
	return false;
}

//ブレンドモードの取得
BlendMode Object3d::GetBlendMode()const{
	return blendMode_;
}

//描画時のトランスフォームモードの取得
RenderTransformMode Object3d::GetRenderTransformMode()const{
	return renderTransformMode_;
}

//マテリアルインスタンスの取得
MaterialInstance* Object3d::GetMaterialInstance(){
	return materialInstance_.get();
}

//マテリアルインスタンスの取得
const MaterialInstance* Object3d::GetMaterialInstance() const{
	return materialInstance_.get();
}

//現在のLODに対応した描画用モデルを取得
Model* Object3d::GetRenderModel(){
	//元モデルがNullの場合
	if (!baseModel_){
		return nullptr;
	}

	return baseModel_->GetLODModel(currentLOD_);
}

//現在のLOD番号を取得
uint32_t Object3d::GetCurrentLOD() const{
	return currentLOD_;
}

//ノードのメッシュインスタンスの取得
const std::vector<Object3d::NodeMeshInstance>& Object3d::GetNodeMeshInstance() const{
	return nodeMeshInstance_;
}

//ノードのローカルトランスフォームの取得
const Transform Object3d::GetNodeLocalTransform(const std::string& path){
	Node* found = FindNode(path);

	//無かった場合
	if (!found){
		return {};
	}

	return found->localTransform;
}

//ノードの名前位一覧を取得
std::vector<Object3d::NodeInfo> Object3d::GetNodeNames()const{
	std::vector<NodeInfo>nodeInfos;

	//モデルがなければ
	if (!baseModel_){
		return nodeInfos;
	}

	//ノードの名前を集める
	CollectNodeNames(node_, "", nodeInfos);

	return nodeInfos;
}

//ノードを取得
const Node& Object3d::GetNode()const{
	return node_;
}

//親ノードのパスを取得
const std::string& Object3d::GetParentNodePath() const{
	return parentNodePath_;
}

//親オブジェクトを取得
const Object3d* Object3d::GetParentObject3d() const{
	return parentObject3d_;
}

//ノードの名前を集める
void Object3d::CollectNodeNames(const Node& node, const std::string& parentPath, std::vector<NodeInfo>& nodeInfos)const{
	//現在のパス
	std::string currentPath = parentPath;

	//ノードの名前が空じゃなければ
	if (!node.name.empty()){
		//現在のパスが空じゃなければ
		if (!currentPath.empty()){
			currentPath += "/";
		}

		//現在のパスにノードの名前を追加
		currentPath += node.name;

		//ノードの情報に追加
		nodeInfos.push_back({
			.name = node.name,
			.path = currentPath,
		});
	}

	for (const Node& child : node.children){
		CollectNodeNames(child, currentPath, nodeInfos);
	}
}

//ノードを探す
Node* Object3d::FindNode(const std::string& path){
	//名前が空だったら
	if (path.empty()){
		return nullptr;
	}

	return FindNodeRecursive(node_, "", path);
}

//ノードを再起関数で探す
Node* Object3d::FindNodeRecursive(Node& node, const std::string& parentPath, const std::string& targetPath){
	//現在のパスを取得
	std::string currentPath = parentPath;

	//ノードの名前が空じゃなければ
	if (!node.name.empty()){
		//現在のパスが空じゃなければ
		if (!currentPath.empty()){
			currentPath += "/";
		}

		//現在のパスにノードの名前を追加
		currentPath += node.name;
	}

	//path全体が一致した
	if (currentPath == targetPath){
		return &node;
	}

	//子の方も探索
	for (Node& child : node.children){
		Node* found = FindNodeRecursive(child, currentPath, targetPath);

		if (found){
			return found;
		}
	}

	return nullptr;
}

//ワールド行列を作成
void Object3d::MakeWorldMatrix(){
	GameObject* gameObject = GetOwner();
	//ゲームオブジェクトがNullじゃないか
	assert(gameObject);

	//親オブジェクトがある場合
	if (parentObject3d_){
		Node* node = parentObject3d_->FindNode(parentNodePath_);
		if (node){
			parentMatrix_ = node->modelMatrix * parentObject3d_->GetWorldMatrix();
		}
	}

	//このオブジェクト本来のワールド行列を求める
	localMatrix_ = matrixUtility::MakeAffineMatrix(gameObject->GetTransform());

	//ワールド行列を求める
	worldMatrix_ = localMatrix_ * parentMatrix_;

	//ノードのメッシュインスタンスをクリア
	nodeMeshInstance_.clear();

	//モデルがなければ
	if (!baseModel_){
		return;
	}

	//Nodeを含めてワールド行列を更新
	UpdateNodeMatrices(node_, Matrix4x4::Identity4x4());
}

//マテリアルを個別化する
void Object3d::EnsureUniqueMaterialInstance(){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}

	//ModelやほかのObject3dと共有中なら個別コピー
	if (materialInstance_.use_count() > 1){
		materialInstance_ = std::make_shared<MaterialInstance>(*materialInstance_);
	}
}

//Nodeの行列を更新
void Object3d::UpdateNodeMatrices(Node& node, const Matrix4x4& parentMatrix){
	//アニメーション用の行列
	Matrix4x4 animationMatrix = matrixUtility::MakeAffineMatrix(node.localTransform);

	//ローカル行列を求める
	node.localMatrix = animationMatrix * node.baseMatrix;

	//階層を累積した行列
	node.modelMatrix = node.localMatrix * parentMatrix;

	//メッシュのインデックス分行列を計算
	for (uint32_t meshIndex : node.meshIndices){
		assert(baseModel_);
		assert(meshIndex < static_cast<uint32_t>(baseModel_->GetMeshes().size()));
		nodeMeshInstance_.push_back({ meshIndex, node.modelMatrix });
	}

	//子の数分回す
	for (Node& child : node.children){
		UpdateNodeMatrices(child, node.modelMatrix);
	}
}
