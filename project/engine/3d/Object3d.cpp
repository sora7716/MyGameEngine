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
	cloneInstance->SetPixelShaderFilePath(this->pixelShaderFilePath_);
	cloneInstance->SetBlendMode(this->blendMode_);
	cloneInstance->SetCullingMode(this->cullingMode_);
	cloneInstance->SetFillMode(this->fillMode_);
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

//プロパティ一覧を登録
void Object3d::RegisterProperties(PropertyRegistry* registry){
	Component::RegisterProperties(registry);
	//マテリアルインスタンスがNullなら
	if (!materialInstance_){
		return;
	}

	//マテリアルを取得
	MaterialInstance* material = materialInstance_.get();

	//ブレンドモードの切り替え
	std::function<uint32_t()> blendModeGetter = [this]() -> uint32_t{
		return static_cast<uint32_t>(this->GetBlendMode());
		};
	std::function<void(uint32_t)> blendModeSetter = [this](uint32_t value){
		this->SetBlendMode(static_cast<BlendMode>(value));
		};
	registry->Add("blendMode", kBlendOptions, blendModeGetter, blendModeSetter);

	//登録
	for (uint32_t i = 0; i < material->GetSlots().size(); i++){
		//グループの開始
		registry->BeginGroup("material" + std::to_string(i));

		//拡散反射モデルの切り替え
		std::function<uint32_t()> diffuseModelGetter = [i, material]()->uint32_t{
			return material->GetSlots()[i].material.diffuseModel;
			};
		std::function<void(uint32_t)> diffuseModelSetter = [i, this](uint32_t diffuseModel){
			return this->SetDiffuseModel(i, static_cast<DiffuseModel>(diffuseModel));
			};
		registry->Add("拡散反射モデル", { "Lambert","HalfLambert" }, diffuseModelGetter, diffuseModelSetter);

		//鏡面反射モデルの切り替え
		std::function<uint32_t()> specularModelGetter = [i, material]()->uint32_t{
			return material->GetSlots()[i].material.specularModel;
			};
		std::function<void(uint32_t)> specularModelSetter = [i, this](uint32_t specularModel){
			return this->SetSpecularModel(i, static_cast<SpecularModel>(specularModel));
			};
		registry->Add("鏡面反射モデル", { "Phong","BlingPhong" }, specularModelGetter, specularModelSetter);
		//色
		registry->Add("色", material->GetSlots()[i].material.color, this, i, &Object3d::SetColor);

		//UVScale
		registry->Add("UVの拡縮", material->GetSlots()[i].uvTransform.scale, this, i, &Object3d::SetUVScale, 0.1f);

		//UVRotate
		registry->Add("UVの角度", material->GetSlots()[i].uvTransform.rotate, this, i, &Object3d::SetUVRotate, 0.1f);

		//UVTranslate
		registry->Add("UVの位置", material->GetSlots()[i].uvTransform.translate, this, i, &Object3d::SetUVTranslate, 0.1f);

		//ライティングするか
		registry->Add("ライティングするか", enabledLighting_, this, i, &Object3d::SetEnabledLighting);

		//輝度
		registry->Add("輝度", material->GetSlots()[i].material.shininess, this, i, &Object3d::SetShininess, 0.1f);

		//環境マップの映り込み
		registry->Add("環境マップの映り込み", material->GetSlots()[i].material.environmentCoefficient, this, i, &Object3d::SetEnvironmentCoefficient, 0.1f);

		//グループの終了
		registry->EndGroup();
	}
}

//親子付けを外す
void Object3d::Detach(){
	//親子付けのパスがなければ
	if (!parentNode_){
		return;
	}

	//パスをクリア
	parentNode_ = nullptr;
	parentWorldMatrix_ = Matrix4x4::Identity4x4();

	//接続されたノードパスをクリア
	attachedNodePath_.clear();
}

//親子付け
bool Object3d::AttachTo(Object3d* parent, const std::string& parentNodePath){
	if (!parent){
		Logger::OutputLog("親子付けを行う際の親がnull参照しました");
		return false;
	}

	Node* found = parent->FindNode(parentNodePath);
	if (!found){
		Logger::OutputLog("存在しないNodeのPathにアクセスしようとしました");
		return false;
	}

	//メンバ変数に記録
	parentNode_ = found;
	parentWorldMatrix_ = parent->GetOwner()->GetWorldMatrix();

	//接続行列を渡す
	GetOwner()->SetAttachedMatrix(parentNode_->modelMatrix);

	//接続する
	bool isAttachTo = GetOwner()->AttachTo(parent->GetOwner());
	//接続が失敗したら
	if (!isAttachTo){
		return false;
	}

	//ノードパスを保存
	attachedNodePath_ = parentNodePath;
	return true;
}

//ワールド行列を作成
Matrix4x4 Object3d::MakeRenderWorldMatrix(const Matrix4x4& cameraWorldMatrix) const{
	//Normalだった場合
	if (renderTransformMode_ == RenderTransformMode::kNormal){
		return GetOwner()->GetWorldMatrix();
	}

	//もしBillboardだった場合
	GameObject* gameObject = GetOwner();
	//ゲームオブジェクトがない場合
	if (!gameObject){
		return GetOwner()->GetWorldMatrix();
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
void Object3d::SetColor(uint32_t index, const Color& color){
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
void Object3d::SetEnabledLighting(uint32_t index, bool enabledLighting){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	enabledLighting_ = enabledLighting;
	materialInstance_->SetIsLighting(index, enabledLighting_);
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
void Object3d::SetEnvironmentCoefficient(uint32_t index, float environmentCoefficient){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetEnvironmentCoefficient(index, environmentCoefficient);
}

//拡散反射モデルの切り替え
void Object3d::SetDiffuseModel(uint32_t index, DiffuseModel diffuseModel){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetDiffuseModel(index, diffuseModel);
}

//鏡面反射モデルの切り替え
void Object3d::SetSpecularModel(uint32_t index, SpecularModel specularModel){
	//マテリアルインスタンスがなければ
	if (!materialInstance_){
		return;
	}
	//マテリアルを個別化する
	EnsureUniqueMaterialInstance();
	materialInstance_->SetSpecularModel(index, specularModel);
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

//ピクセルシェーダファイルパスの設定
void Object3d::SetPixelShaderFilePath(const std::string& pixelShaderFilePath){
	pixelShaderFilePath_ = pixelShaderFilePath;
}

//ブレンドモードの設定
void Object3d::SetBlendMode(BlendMode blendMode){
	blendMode_ = blendMode;
}

//カリングモードの設定
void Object3d::SetCullingMode(rasterizerMode::CullingMode cullingMode){
	cullingMode_ = cullingMode;
}

//フィルモードの設定
void Object3d::SetFillMode(rasterizerMode::FillMode fillMode){
	fillMode_ = fillMode;
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

//コンポーネントのタイプ名を返す
std::string Object3d::GetTypeName() const{
	return "Object3d";
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

//ピクセルシェーダファイルパスの取得
const std::string& Object3d::GetPixelShaderFilePath() const{
	return pixelShaderFilePath_;
}

//ブレンドモードの取得
BlendMode Object3d::GetBlendMode()const{
	return blendMode_;
}

//カリングモードの取得
rasterizerMode::CullingMode Object3d::GetCullingMode() const{
	return cullingMode_;
}

//フィルモードの取得
rasterizerMode::FillMode Object3d::GetFillMode() const{
	return fillMode_;
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

//ノードの行列を取得できるか試す
bool Object3d::TryGetNodeModelMatrix(const std::string& path, Matrix4x4& outMatrix){
	Node* found = FindNode(path);

	//無かった場合
	if (!found){
		return false;
	}

	//モデル行列を代入
	outMatrix = found->modelMatrix;
	return true;
}

//ノードを取得できるか試す
bool Object3d::TryGetNodeForPath(const std::string& path, Node& node){
	Node* found = FindNode(path);

	//無かった場合
	if (!found){
		return false;
	}

	//モデル行列を代入
	node = *found;
	return true;
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

//接続されたノードのパスを取得
const std::string& Object3d::GetAttachedNodePath() const{
	return attachedNodePath_;
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
	if (parentNode_){
		gameObject->SetAttachedMatrix(parentNode_->modelMatrix);
	}

	//ワールド行列を求める
	//worldMatrix_ = localMatrix_ * parentMatrix_;

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
