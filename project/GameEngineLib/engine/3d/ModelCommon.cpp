#include "ModelCommon.h"
#include "engine/base/DirectXBase.h"
#include "engine/2d/TextureManager.h"

//初期化
void ModelCommon::Initialize(DirectXBase* directXBase, TextureManager* textureManager) {
	//DirectXの基盤部分の記録
	directXBase_ = directXBase;
	//テクスチャマネージャーの記録
	textureManager_ = textureManager;
}

// DirectXの基盤部分のゲッター
DirectXBase* ModelCommon::GetDirectXBase() const {
	return directXBase_;
}

//テクスチャマネージャーのゲッター
TextureManager* ModelCommon::GetTextureManager() {
	return textureManager_;
}
