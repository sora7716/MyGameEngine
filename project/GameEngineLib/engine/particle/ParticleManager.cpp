#include "ParticleManager.h"
//デストラクタ
ParticleManager::~ParticleManager() {
}

//パーティクルシステムの追加
void ParticleManager::AddParticleSystem(const std::string& name) {
	//読み込み済みならカメラを検索
	if (particleSystems_.contains(name)) {
		//読み込み済みなら早期return
		return;
	}
	//パーティクルシステムの生成
	std::unique_ptr<ParticleSystem>particleSystems = std::make_unique<ParticleSystem>();

	//パーティクルシステムのmapコンテナに格納する
	particleSystems_.insert(std::make_pair(name, std::move(particleSystems)));
}

//パーティクルシステムの検索
ParticleSystem* ParticleManager::FindParticleSystem(const std::string& name) {
	//読み込み済みモデルを検索
	if (particleSystems_.contains(name)) {
		//読み込み済みモデルを戻り値としてreturn
		return particleSystems_.at(name).get();
	}
	//ファイル名一致なし
	return nullptr;
}

//コンストラクタ
ParticleManager::ParticleManager(ConstructorKey) {
}
