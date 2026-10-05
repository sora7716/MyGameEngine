#pragma once
#include "MatrixUtility.h"
#include "BlendMode.h"
#include "RenderingData.h"
#include "Object3dRenderData.h"
#include "Component.h"
#include <vector>
#include <string>
#include <memory>

//前方宣言
class Model;
class GameObject;
class MaterialInstance;
class LODController;

/// <summary>
/// 3Dオブジェクト
/// </summary>
class Object3d :public Component{
public://構造体
	//メッシュのインスタンス
	struct NodeMeshInstance{
		uint32_t meshIndex = 0;
		Matrix4x4 nodeMatrix = Matrix4x4::Identity4x4();
	};

	//ノードの情報
	struct NodeInfo{
		std::string name;
		std::string path;
	};
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit Object3d(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Object3d()override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 複製
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <returns>コンポーネント</returns>
	std::unique_ptr<Component>Clone(GameObject* gameObject)const override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// カメラとの距離からLODを更新
	/// </summary>
	/// <param name="distance">カメラとの距離</param>
	void UpdateLOD(float distance);

	/// <summary>
	/// 親子付けを外す
	/// </summary>
	void DetachParent();

	/// <summary>
	/// 親子付け
	/// </summary>
	/// <param name="parent">親</param>
	/// <param name="parentNodePath">親となるノードのパス</param>
	/// <returns>成功したか</returns>
	bool AttachTo(Object3d* parent, const std::string& parentNodePath);

	/// <summary>
	/// ワールド行列を作成
	/// </summary>
	/// <param name="cameraWorldMatrix">カメラのワールド行列</param>
	/// <returns>ワールド行列</returns>
	Matrix4x4 MakeRenderWorldMatrix(const Matrix4x4& cameraWorldMatrix)const;

	/// <summary>
	/// モデルの設定
	/// </summary>
	/// <param name="modelName">モデル名</param>
	void SetModel(const std::string& modelName);

	/// <summary>
	/// LODの切り替え距離の設定
	/// </summary>
	/// <param name="lodDistances">lod切り替え距離</param>
	void SetLODDistances(const std::vector<float>& lodDistances);

	/// <summary>
	/// ヒステリシス幅の設定
	/// </summary>
	/// <param name="hysteresis">ヒステリシス幅</param>
	void SetHysteresis(float hysteresis);

	/// <summary>
	/// uvスケールの設定
	/// </summary>
	/// <param name="index">マテリアルスロット番号の検索キー</param>
	/// <param name="uvScale">スケール</param>
	void SetUVScale(uint32_t index, const Vector2& uvScale);

	/// <summary>
	/// uv回転の設定
	/// </summary>
	/// <param name="index">マテリアルスロット番号の検索キー</param>
	/// <param name="uvRotate">回転</param>
	void SetUVRotate(uint32_t index, float uvRotate);

	/// <summary>
	/// uv平行移動の設定
	/// </summary>
	/// <param name="index">マテリアルスロット番号の検索キー</param>
	/// <param name="uvTranslate">平行移動</param>
	void SetUVTranslate(uint32_t index, const Vector2& uvTranslate);

	/// <summary>
	/// 色の設定
	/// </summary>
	/// <param name="index">マテリアルスロット番号の検索キー</param>
	/// <param name="color">色</param>
	void SetColor(uint32_t index, const Vector4& color);

	/// <summary>
	/// テクスチャの設定
	/// </summary>
	/// <param name="index">マテリアルスロット番号の検索キー</param>
	/// <param name="imageFileName">画像のファイル名</param>
	void SetTexture(uint32_t index, const std::string& imageFileName);

	/// <summary>
	/// 環境マップの設定
	/// </summary>
	/// <param name="index">マテリアルスロット番号の検索キー</param>
	/// <param name="environmentMapFileName">環境マップのファイル名</param>
	void SetEnvironmentMap(uint32_t index, const std::string& environmentMapFileName);

	/// <summary>
	/// ライティングフラグの設定
	/// </summary>
	/// <param name="index">マテリアルスロット番号の検索キー</param>
	/// <param name="isLighting">ライティングフラグ</param>
	void SetIsLighting(uint32_t index, bool isLighting);

	/// <summary>
	/// 輝度の設定
	/// <param name="index">マテリアルスロット番号の検索キー</param>
	/// </summary>
	/// <param name="shininess">輝度</param>
	void SetShininess(uint32_t index, float shininess);

	/// <summary>
	/// 環境マップの映り込み度を調整
	/// </summary>
	/// <param name="index">マテリアルスロット番号の検索キー</param>
	/// <param name="environmentCoefficient">k環境マップの映り込み度</param>
	void SetEnvironmentCoefficient(uint32_t index, float& environmentCoefficient);

	/// <summary>
	/// UV座標の設定
	/// </summary>マテリアルスロット番号の検索キー
	/// <param name="index">マテリアルスロット番号の検索キー</param>
	/// <param name="uvTransform">UV座標</param>
	void SetUVTransform(uint32_t index, const RectTransform& uvTransform);

	/// <summary>
	/// ブレンドモードの設定
	/// </summary>
	/// <param name="blendMode">ブレンドモード</param>
	void SetBlendMode(BlendMode blendMode);

	/// <summary>
	/// 描画時のトランスフォームモードの設定
	/// </summary>
	/// <param name="transformMode">描画時のトランスフォームモード</param>
	void SetRenderTransformMode(RenderTransformMode transformMode);

	/// <summary>
	/// ノードのローカルトランスフォームの設定
	/// </summary>
	/// <param name="path">ノードのパス名</param>
	/// <param name="localTransform">ローカルトランスフォーム</param>
	/// <returns>Nodeの取得に成功したか</returns>
	bool SetNodeLocalTransform(const std::string& path, const Transform& localTransform);

	/// <summary>
	/// ワールド行列の取得
	/// </summary>
	/// <returns>ワールド行列</returns>
	const Matrix4x4& GetWorldMatrix()const;

	/// <summary>
	/// ワールド座標の取得
	/// </summary>
	/// <param name="instanceIndex">インスタンスのマテリアルスロット番号の検索キー</param>
	/// <returns>ワールド座標</returns>
	Vector3 GetWorldPos();

	/// <summary>
	/// メッシュのサイズの取得
	/// </summary>
	/// <returns>メッシュのサイズ</returns>
	uint32_t GetMeshDataSize();

	/// <summary>
	/// モデルの取得
	/// </summary>
	/// <returns>モデル</returns>
	Model* GetModel();

	/// <summary>
	/// モデルの取得
	/// </summary>
	/// <returns>モデル</returns>
	const Model* GetModel()const;

	/// <summary>
	/// モデルが設定されているかどうか
	/// </summary>
	/// <returns>モデルが設定されているかどうか</returns>
	bool HasModel()const;

	/// <summary>
	/// ブレンドモードの取得
	/// </summary>
	/// <returns>ブレンドモード</returns>
	BlendMode GetBlendMode()const;

	/// <summary>
	/// 描画時のトランスフォームモードの取得
	/// </summary>
	/// <returns>描画時のトランスフォームモード</returns>
	RenderTransformMode GetRenderTransformMode()const;

	/// <summary>
	/// マテリアルインスタンスの取得
	/// </summary>
	/// <returns>マテリアルインスタンス</returns>
	MaterialInstance* GetMaterialInstance();

	/// <summary>
	/// マテリアルインスタンスの取得
	/// </summary>
	/// <returns>マテリアルインスタンス</returns>
	const MaterialInstance* GetMaterialInstance()const;

	/// <summary>
	/// 現在のLODに対応した描画用モデルを取得
	/// </summary>
	/// <returns>描画用モデル</returns>
	Model* GetRenderModel();

	/// <summary>
	/// 現在のLOD番号を取得
	/// </summary>
	/// <returns>現在のLOD番号</returns>
	uint32_t GetCurrentLOD()const;

	/// <summary>
	/// ノードのメッシュインスタンスの取得
	/// </summary>
	/// <returns>ノードのメッシュインスタンス</returns>
	const std::vector<NodeMeshInstance>& GetNodeMeshInstance()const;

	/// <summary>
	/// ノードのローカルトランスフォームの取得
	/// </summary>
	/// <param name="path">ノードのパス</param>
	/// <returns>ノードのローカルトランスフォーム</returns>
	const Transform GetNodeLocalTransform(const std::string& path);

	/// <summary>
	/// ノードの名前一覧を取得
	/// </summary>
	/// <returns>ノードの情報の配列</returns>
	std::vector<Object3d::NodeInfo>GetNodeNames()const;

	/// <summary>
	/// ノードを取得
	/// </summary>
	/// <returns>ノード</returns>
	const Node& GetNode()const;

	/// <summary>
	/// 親ノードのパスを取得
	/// </summary>
	/// <returns>親ノードのパスを取得</returns>
	const std::string& GetParentNodePath()const;

	/// <summary>
	/// 親オブジェクトを取得
	/// </summary>
	/// <returns>親オブジェクト</returns>
	const Object3d* GetParentObject()const;
private://メンバ関数
	/// <summary>
	/// ノードの名前を集める
	/// </summary>
	/// <param name="node">ノード</param>
	/// <param name="parentPath">親のパス</param>
	/// <param name="nodeInfos">ノードの情報を集める配列</param>
	void CollectNodeNames(const Node& node, const std::string& parentPath, std::vector<NodeInfo>& nodeInfos)const;

	/// <summary>
	/// ノードを探す
	/// </summary>
	/// <param name="name">ノードのパス</param>
	/// <returns>ノード</returns>
	Node* FindNode(const std::string& path);

	/// <summary>
	/// ノードを再起関数で探す
	/// </summary>
	/// <param name="node">ノード</param>
	/// <param name="parentPath">親のパス</param>
	/// <param name="targetPath">対象のパス</param>
	/// <returns>ノード</returns>
	Node* FindNodeRecursive(Node& node, const std::string& parentPath, const std::string& targetPath);

	/// <summary>
	/// ワールド行列を作成
	/// </summary>
	void MakeWorldMatrix();

	/// <summary>
	/// マテリアルを個別化する
	/// </summary>
	void EnsureUniqueMaterialInstance();

	/// <summary>
	/// Nodeの行列を更新
	/// </summary>
	/// <param name="node">ノード</param>
	/// <param name="parentMatrix">親行列</param>
	void UpdateNodeMatrices(Node& node, const Matrix4x4& parentMatrix);
private://定数
	//インスタンスの最大数
	static const inline uint32_t kMaxInstanceCount_ = 1024;
private://メンバ変数
	//ブレンドモード
	BlendMode blendMode_ = BlendMode::kNone;

	//モデル
	Model* baseModel_ = nullptr;

	//このObject3dが使用するマテリアル
	std::shared_ptr<MaterialInstance>materialInstance_ = nullptr;

	//今現在のLOD番号
	uint32_t currentLOD_ = 0;
	//LODの制御
	std::unique_ptr<LODController>lodController_ = nullptr;

	//オブジェクトの見た目
	RenderTransformMode renderTransformMode_ = RenderTransformMode::kNormal;

	//親
	Object3d* parentObject_ = nullptr;
	std::string  parentNodePath_ = "";

	//親行列
	Matrix4x4 parentMatrix_ = {};
	//ローカル行列
	Matrix4x4 localMatrix_ = {};
	//ワールド行列
	Matrix4x4 worldMatrix_ = {};

	//ノード
	Node node_ = {};
	//メッシュのワールド行列
	std::vector<NodeMeshInstance>nodeMeshInstance_;
};