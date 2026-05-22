#include "AudioManager.h"
//デストラクタ
AudioManager::~AudioManager() {
	//オーディオ達の解放
	for (auto& [name, audio] : audios_) {
		audio->Finalize();
	}
}
//オーディオの追加
void AudioManager::LoadAudio(const std::string& name, const std::string& filename) {
	//読み込み済みのオーディオを検索
	if (audios_.contains(name)) {
		//読み込み済みなら早期リターン
		return;
	}
	//オーディオの生成
	std::unique_ptr<Audio> audio = std::make_unique<Audio>();
	audio->Initialize();
	audio->SetSoundData(filename);

	//オーディオをmapコンテナに格納する
	audios_.insert(std::make_pair(name, std::move(audio)));
}

//オーディオの検索
Audio* AudioManager::FindAudio(const std::string& name) {
	//オーディオの検索
	if (audios_.contains(name)) {
		return audios_.at(name).get();
	}
	//ファイルなし
	return nullptr;
}

//コンストラクタ
AudioManager::AudioManager(ConstructorKey) {
}