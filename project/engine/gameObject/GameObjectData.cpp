#include "GameObjectData.h"

//初期化
void GameObject::Initialize() {
	transformData.Initialize();
	isActive = true;
	isEnabled = true;
	tag = Tag::kNone;
}
