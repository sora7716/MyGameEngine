#pragma once
#include <xaudio2.h>
#include <fstream>
#include <wrl.h>
#include <string>

/// <summary>
/// 音楽関係のクラス
/// </summary>
class Audio {
public://構造体等
	//チャンクヘッダ
	typedef struct ChunkHeader {
		char id[4];//チャンク毎のID
		int32_t size;//チャンクサイズ
	}ChunkHeader;

	//RIFFヘッダチャンク
	typedef struct RiffHeader {
		ChunkHeader chunk;//"RIFF"
		char type[4];//"WAVE"
	}RiffHeader;

	//FMTチャンク
	typedef struct FormatChunk {
		ChunkHeader chunk;//"fmt"
		WAVEFORMATEX fmt;//波形フォーマット
	}FormatChunk;

	//音声データ
	typedef struct SoundData {
		WAVEFORMATEX wfex;//波形フォーマット
		BYTE* pBuffer;//バッファの先頭アドレス
		unsigned int bufferSize;//バッファサイズ
	}SoundData;

public://メンバ関数
	/// <summary>
    /// コンストラクタ
    /// </summary>
	Audio() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Audio() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directoryPath">ディレクトリ名</param>
	void Initialize(const std::string& directoryPath = "engine/resources/sounds");

	/// <summary>
	/// サウンドデータのセッター
	/// </summary>
	/// <param name="filename">ファイル名</param>
	void SetSoundData(const std::string& filename);

	/// <summary>
	/// サウンドデータの再生
	/// </summary>
	/// <param name="isLoop">ループさせるかのフラグ</param>
	void SoundPlayWave(bool isLoop);

	/// <summary>
	/// 最後にやるやつ
	/// </summary>
	void Finalize();

	/// <summary>
	/// 音量のセッター
	/// </summary>
	/// <param name="volume">音量</param>
	void SetVolume(float volume = 1.0f);

	/// <summary>
	/// 再度再生
	/// </summary>
	void ReplayAudio();

	/// <summary>
	/// 一時停止
	/// </summary>
	void PauseAudio();

	/// <summary>
	/// 完全停止
	/// </summary>
	void StopAudio();

private://メンバ関数
	/// <summary>
	/// サウンドデータのロード
	/// </summary>
	/// <param name="filename">ファイル名</param>
	/// <returns></returns>
	SoundData SoundLoadWave(const std::string& filename);

	/// <summary>
	/// 音声データの解放
	/// </summary>
	/// <param name="soundData">サウンドデータ</param>
	void SoundUnLoad(SoundData* soundData);
private://メンバ変数
	//XAudio2
	Microsoft::WRL::ComPtr<IXAudio2>xAudio2_;
	//マスターボイス
	IXAudio2MasteringVoice* masterVoice_;
	//ディレクトリ名
	std::string directoryPath_ = "";
	//サウンドデータ
	SoundData soundData_ = {};
	//波形フォーマットを元にSourceVoiceの生成
	IXAudio2SourceVoice* pSorceVoice_ = nullptr;
};

