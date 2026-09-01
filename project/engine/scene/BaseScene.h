#pragma once
#include "Context.h"
#include "Input.h"
#include "DebugCamera.h"
#include "Camera.h"
#include <memory>

// 前方宣言
class DirectXBase;
class AbstractSceneFactory;
class ColliderManager;
class GameObject;
class Object3dRenderer;
class SkyBoxRenderer;
class DebugDrawRenderer;
class ParticleRenderer;

/// <summary>
/// シーンの基底クラス
/// </summary>
class BaseScene{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	BaseScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BaseScene();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// デバッグ
	/// </summary>
	virtual void Debug();

	/// <summary>
	/// 終了
	/// </summary>
	virtual void Finalize();

	/// <summary>
	/// ゲームオブジェクトの削除
	/// </summary>
	/// <param name="target">対象となるゲームオブジェクト</param>
	void DeleteGameObject(GameObject* target);

	/// <summary>
	/// ゲームオブジェトの複製
	/// </summary>
	/// <param name="target">対象となるゲームオブジェクト</param>
	void DuplicateGameObject(GameObject* target);

	/// <summary>
	/// ゲームオブジェクトの位置(配列の順番)の変更
	/// </summary>
	/// <param name="from">今いる場所</param>
	/// <param name="to">最終的に置いておく場所</param>
	void MoveGameObject(uint32_t from, uint32_t to);

	/// <summary>
	/// ゲームオブジェクトのタグを古いのから新しいのに変更
	/// </summary>
	/// <param name="oldTag">古い名前</param>
	/// <param name="newTag">新しい名前</param>
	void ReplaceGameObjectTag(const std::string& oldTag, const std::string& newTag);

	/// <summary>
	/// 空のゲームオブジェクトを生成
	/// </summary>
	/// <returns>空のゲームオブジェクト</returns>
	GameObject* CreateGameObject();

	/// <summary>
	/// ゲームオブジェクトの一覧を取得
	/// </summary>
	/// <returns>ゲームオブジェクト</returns>
	const std::vector<std::unique_ptr<GameObject>>& GetGameObjects()const;

	/// <summary>
	/// シーンで必要な情報の設定
	/// </summary>
	/// <param name="sceneContext">シーンに必要な情報</param>
	void SetSceneContext(const SceneContext& sceneContext);
	
	/// <summary>
	/// ゲームカメラの取得
	/// </summary>
	/// <returns>ゲームカメラ</returns>
	Camera* GetGameCamera();
private://メンバ関数
	/// <summary>
	/// 名前を重複しないようにする
	/// </summary>
	/// <param name="baseName">元の名前</param>
	/// <param name="remove">省きたい部分</param>
	std::string CreateUniqueGameObjectName(const std::string& baseName, std::string_view remove)const;
private://定数
	//オブジェクトの大きさ
	static inline const uint32_t kGameObjectSize = 65536;
protected://メンバ変数
	//Xboxの番号
	DWORD xBoxPadNumber_ = 0;
	//シーンで必要なもの
	SceneContext sceneContext_ = {};
	//シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;
	//コライダーマネージャー
	//std::unique_ptr<ColliderManager>colliderManager_ = nullptr;
	//ゲームプレイ用のカメラ
	Camera* gameCamera_ = nullptr;
	//ゲームオブジェクトの一覧
	std::vector<std::unique_ptr<GameObject>> gameObjects_;
};

