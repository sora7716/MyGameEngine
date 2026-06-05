#include "GameObjectData.h"

//初期化
void GameObject::Initialize() {
	transform.Initialize();
	isActive = true;
	isEnabled = true;
	tag = Tag::kNone;
	currentLOD = 0;
}
