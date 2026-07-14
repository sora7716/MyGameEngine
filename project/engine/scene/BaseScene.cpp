#include "BaseScene.h"
#include "DebugCamera.h"
#include "AbstractSceneFactory.h"
#include "GlobalVariables.h"
#include "WinApi.h"
#include "GameObject.h"
#include "StringUtility.h"
#include <algorithm>
//#include "algorithms/ColliderManager.h"

//コンストラクタ
BaseScene::BaseScene(){
}

//デストラクタ
BaseScene::~BaseScene(){
}

//初期化
void BaseScene::Initialize(const SceneContext& sceneContext){
	//ゲームエンジンの核
	sceneContext_ = sceneContext;
	//デバックカメラ
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(sceneContext_.input, sceneContext_.cameraManager);
	//ゲームオブジェクトの大きさを確保しておく(要素数は増えない)
	gameObjects_.reserve(kGameObjectSize);
	//コライダーマネージャー
	//colliderManager_ = std::make_unique<ColliderManager>();
	////調整ファイルの読み込み
	//GlobalVariables::GetInstance()->LoadFiles();
}

//更新
void BaseScene::Update(){
	//デバックカメラ
	debugCamera_->Update();
	//コライダーマネージャー
	//colliderManager_->ProcessCollision();
}

//デバッグ
void BaseScene::Debug(){

}

//終了
void BaseScene::Finalize(){
	//シーンファクトリーの解放
	delete sceneFactory_;
	sceneFactory_ = nullptr;
}

//ゲームオブジェクトの削除
void BaseScene::DeleteGameObject(GameObject* target){
	gameObjects_.erase(
		std::remove_if(
			gameObjects_.begin(),
			gameObjects_.end(),
			[target](const std::unique_ptr<GameObject>& gameObject){
				return gameObject.get() == target;
			}
		),
		gameObjects_.end()
	);
}

//ゲームオブジェトの複製
void BaseScene::DuplicateGameObject(GameObject* target){
	//対象がいなければ
	if (!target){
		return;
	}

	//重複してないかを見て名前を作成
	std::string newName = CreateUniqueGameObjectName(target->GetName(), "_copy");

	//コピーするゲームオブジェクトを作成
	std::unique_ptr<GameObject>duplicate = target->Clone();
	//名前を設定
	duplicate->SetName(newName);

	//ゲームオブジェクトに追加
	gameObjects_.push_back(duplicate->Clone());
}

//空のゲームオブジェクトを生成
GameObject* BaseScene::CreateGameObject(){
	//重複してない名前を生成
	std::string name = CreateUniqueGameObjectName("GameObjectr", "_");

	//GameObjectの生成
	std::unique_ptr<GameObject>gameObject = GameObject::Create(name);

	//ポインタを保存
	GameObject* gameObjectPtr = gameObject.get();

	//ゲームオブジェクトに追加
	gameObjects_.push_back(std::move(gameObject));

	return gameObjectPtr;
}

//ゲームオブジェクトの取得
const std::vector<std::unique_ptr<GameObject>>& BaseScene::GetGameObjects()const{
	// TODO: return ステートメントをここに挿入します
	return gameObjects_;
}

//名前を重複しないようにする
std::string BaseScene::CreateUniqueGameObjectName(const std::string& baseName, std::string_view remove)const{
	std::string name = baseName;

	//removeを取り除いた元の名前
	name = stringUtility::RemoveSuffix(baseName, remove);
	uint32_t count = 1;
	while (true){
		//新しい候補の名前
		std::string candidate = name + static_cast<std::string>(remove) + std::to_string(count);

		bool alreadyExists = false;
		for (const std::unique_ptr<GameObject>& gameObject : gameObjects_){
			//ゲームオブジェクトが存在していなかったら
			if (!gameObject){
				continue;
			}
			//一致していたら
			if (gameObject->GetName() == candidate){
				alreadyExists = true;
				break;
			}
		}

		if (!alreadyExists){
			return candidate;
		}

		count++;
	}


	return name + std::to_string(count);
}
