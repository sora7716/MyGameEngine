#include "Audio.h"
#include <cassert>
#pragma comment(lib,"xaudio2.lib")

//初期化
void Audio::Initialize(const std::string& directoryPath) {
	//ディレクトリ名を受け取る
	directoryPath_ = directoryPath;
	HRESULT result = S_FALSE;
	//XAudioエンジンのインスタンスを生成
	result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	//マスターボイスを生成
	result = xAudio2_->CreateMasteringVoice(&masterVoice_);
}

//サウンドデータのセッター
void Audio::SetSoundData(const std::string& filename) {
	soundData_ = SoundLoadWave(filename);
}

//サウンドデータの再生
void Audio::SoundPlayWave(bool isLoop) {
	HRESULT result;

	result = xAudio2_->CreateSourceVoice(&pSorceVoice_, &soundData_.wfex);
	assert(SUCCEEDED(result));
	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData_.pBuffer;
	buf.AudioBytes = soundData_.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = isLoop ? XAUDIO2_LOOP_INFINITE : 0;

	//波形データの再生
	result = pSorceVoice_->SubmitSourceBuffer(&buf);
	result = pSorceVoice_->Start();
	assert(SUCCEEDED(result));
}
//最後にやるやつ
void Audio::Finalize() {
	xAudio2_.Reset();
	SoundUnLoad(&soundData_);
}

//音量のセッター
void Audio::SetVolume(float volume) {
	HRESULT result = S_FALSE;
	result = pSorceVoice_->SetVolume(volume);
	assert(SUCCEEDED(result));
}

//再度再生
void Audio::ReplayAudio() {
	HRESULT result = S_FALSE;
	result = pSorceVoice_->Start();
	assert(SUCCEEDED(result));
}

//一時停止
void Audio::PauseAudio() {
	HRESULT result = S_FALSE;
	result = pSorceVoice_->Stop();
	assert(SUCCEEDED(result));
}

//完全停止
void Audio::StopAudio() {
	HRESULT result = S_FALSE;
	result = pSorceVoice_->Stop();
	result = pSorceVoice_->FlushSourceBuffers();
	assert(SUCCEEDED(result));
}

//サウンドデータのロード
Audio::SoundData Audio::SoundLoadWave(const std::string& filename) {
	//1.ファイルオープン
		//ファイル入力ストリームのインスタンス
	std::ifstream file;
	//.wavファイルをバイナリーモードで開く
	file.open(directoryPath_ + "/" + filename+".wav", std::ios_base::binary);
	//ファイルオープン失敗を検知
	assert(file.is_open());
	//2..wavデータの読み込み
	//RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	//ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	//タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4)) {
		assert(0);
	}
	//Formatチャンクの読み込み
	FormatChunk format = {};
	//チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}
	//チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);
	///Dataチャンクの読み込み
	ChunkHeader data;
	while (true) {
		file.read(reinterpret_cast<char*>(&data), sizeof(data));

		// "data" チャンクならループを抜ける
		if (strncmp(data.id, "data", 4) == 0) {
			break;
		}

		// それ以外のチャンクならスキップ
		file.seekg(data.size, std::ios_base::cur);
	}
	//Dataチャンクのデータ部(波形データ)の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);
	//3.ファイルクローズ
	//Waveファイルを閉じる
	file.close();
	//4.読み込んだ音声データをreturn
	//returnする為の音声データ
	SoundData soundData = {};
	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;
	return soundData;
}

//サウンドデータの解放
void Audio::SoundUnLoad(SoundData* soundData) {
	//バッファのメモリ解放
	delete[] soundData->pBuffer;
	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}
