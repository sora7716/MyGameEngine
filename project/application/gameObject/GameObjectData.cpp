#include "GameObjectData.h"

//初期化
void GameObject::Initialize() {
	transformData.Initialize();
	isAlive = true;
	tag = Tag::kNone;
}
