#include "BaseScene.h"
#include "AbstractSceneFactory.h"
#include "WinApi.h"
#include "DebugCameraController.h"
#include "GameObject.h"
#include "StringUtility.h"
#include "TagManager.h"
#include "Input.h"
#include "Object3d.h"
#include "CollisionSystem.h"
#include <algorithm>

//コンストラクタ
BaseScene::BaseScene(){
}

//デストラクタ
BaseScene::~BaseScene(){
}

//初期化
void BaseScene::Initialize(){
	//ゲームオブジェクトの大きさを確保しておく(要素数は増えない)
	gameObjects_.reserve(kGameObjectSize);
	//ゲームオブジェクトのタグを見て、タグのマネージャに存在しているか確認
	for (std::unique_ptr<GameObject>& gameObject : gameObjects_){
		//ゲームオブジェクトがNUllだった場合
		if (!gameObject){
			continue;
		}

		if (!sceneContext_.tagManager->IsContainsTag(gameObject->GetTag())){
			//ゲームオブジェクトのタグがタグの管理になかった場合
			gameObject->SetTag(TagManager::kDefaultTagName);
		}
	}

	//デバッグカメラ
	GameObject* debugCameraObject = CreateGameObject();
	debugCamera_ = debugCameraObject->AddComponent<Camera>();
	debugCameraController_ = debugCameraObject->AddComponent<DebugCameraController>();
	debugCameraObject->SetName("debugCamera");
}

//更新
void BaseScene::Update(){
	//更新のステート
	UpdateState();

	//ゲームオブジェクトのコンポーネントの更新
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects_){
		//gameObjectがNullじゃなければ
		if (gameObject){
			gameObject->UpdateComponents(UpdatePhase::kMain);
		}
	}
}

//更新のステート
void BaseScene::UpdateState(){
}

//デバッグ
void BaseScene::Debug(){
	//ゲームオブジェクトのコンポーネントの更新
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects_){
		//gameObjectがNullじゃなければ
		if (gameObject){
			gameObject->UpdateComponents(UpdatePhase::kDebug);
			gameObject->DebugImGui();
		}
	}
}

//終了
void BaseScene::Finalize(){
	//シーンファクトリーの解放
	sceneFactory_.reset();
}

//ゲームオブジェクトの削除
void BaseScene::DeleteGameObject(GameObject* target, CollisionSystem* collisionSystem){
	//Object3dが存在したら
	Object3d* targetObject3d = target->GetComponent<Object3d>();
	if (targetObject3d){
		//親子関係の解消
		for (std::unique_ptr<GameObject>& gameObject : gameObjects_){
			//ゲームオブジェクトがNullなら
			if (!gameObject){
				continue;
			}

			//Object3dがNullなら
			Object3d* object3d = gameObject->GetComponent<Object3d>();
			if (!object3d){
				continue;
			}

			//親オブジェクトと一致していたら
			if (object3d->GetParentObject3d() == targetObject3d){
				object3d->DetachParent();
			}
		}
	}

	//衝突判定システムから削除
	collisionSystem->RemoveReflectionTo(target);

	//GameObjectからコンポーネントを外すのを通知
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects_){
		//ゲームオブジェクトが存在するか
		if (gameObject){
			//ゲームオブジェクトが対象と一致しているか
			if (gameObject.get() != target){
				gameObject->NotifyGameObjectRemoving(target);
			}
		}
	}

	//ゲームオブジェクトから削除
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
	gameObjects_.push_back(std::move(duplicate));
}

//ゲームオブジェクトの位置(配列の順番)の変更
void BaseScene::MoveGameObject(uint32_t from, uint32_t to){
	//ゲームオブジェクトの配列の開始のイテレータを取得
	std::vector<std::unique_ptr<GameObject>>::iterator begin = gameObjects_.begin();

	//fromとtoが同じ場合
	if (from == to){
		return;
	}

	//場所の入れ替え
	if (to > from){
		//後ろへ移動する場合
		std::rotate(begin + from, begin + from + 1, begin + to + 1);
	} else{
		//前へ移動する場合
		std::rotate(begin + to, begin + from, begin + from + 1);
	}
}

//ゲームオブジェクトのタグを古いのから新しいのに変更
void BaseScene::ReplaceGameObjectTag(const std::string& oldTag, const std::string& newTag){
	//ゲームオブジェクトのタグを見て、タグのマネージャに存在しているか確認
	for (std::unique_ptr<GameObject>& gameObject : gameObjects_){
		//ゲームオブジェクトがNUllだった場合
		if (!gameObject){
			continue;
		}

		if (gameObject->GetTag() == oldTag){
			//ゲームオブジェクトのタグが古かった場合
			gameObject->SetTag(newTag);
		}
	}
}

//空のゲームオブジェクトを生成
GameObject* BaseScene::CreateGameObject(){
	//重複してない名前を生成
	std::string name = CreateUniqueGameObjectName("GameObjectr", "_");

	//GameObjectの生成
	std::unique_ptr<GameObject>gameObject = GameObject::Create(name);

	//現在のシーンを設定
	gameObject->SetCurrentScene(this);

	//ポインタを保存
	GameObject* gameObjectPtr = gameObject.get();

	//ゲームオブジェクトに追加
	gameObjects_.push_back(std::move(gameObject));

	return gameObjectPtr;
}

//ゲームオブジェクトの取得
const std::vector<std::unique_ptr<GameObject>>& BaseScene::GetGameObjects()const{
	return gameObjects_;
}

//セットアップ
void BaseScene::SetUp(const SceneContext& sceneContext){
	sceneContext_ = sceneContext;
}

//シーンで必要な情報の取得
const SceneContext& BaseScene::GetSceneContext(){
	return sceneContext_;
}

//デバッグが有効かの設定
void BaseScene::SetIsDebugControlEnabled(bool isControlEnabled){
	debugCameraController_->SetIsDebugControlEnabled(isControlEnabled);
}

//アプリケーションが有効かの設定
void BaseScene::SetIsAppInputEnabled(bool isAppInputEnabled){
	sceneContext_.input->SetIsAppInputEnabled(isAppInputEnabled);
}

//デバッグカメラの取得
Camera* BaseScene::GetDebugCamera() const{
	return debugCamera_;
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
