#pragma once
#include "Audio.h"
#include <map>
#include <memory>
#include <string>
/// <summary>
/// Audioの管理
/// </summary>
class AudioManager{
public://メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~AudioManager();

	/// <summary>
	/// オーディオのロード
	/// </summary>
	/// <param name="name">名前</param>
	/// <param name="filename">ファイルネーム</param>
	void LoadAudio(const std::string& name,const std::string& filename);

	/// <summary>
	/// オーディオの検索
	/// </summary>
	/// <param name="name">名前</param>
	/// <returns>オーディオ</returns>
	Audio* FindAudio(const std::string& name);
public://PrassKey
	class ConstructorKey {
	private:
		ConstructorKey() = default;
		friend class Core;
	};
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PrassKeyを受け取る</param>
	explicit 	AudioManager(ConstructorKey);
private://メンバ関数
	//コピーコンストラクタ禁止
	AudioManager(const AudioManager&) = delete;
	//代入演算子の禁止
	AudioManager operator=(const AudioManager&) = delete;
private://静的メンバ変数
	//インスタンス
	static inline AudioManager* instance = nullptr;
	//終了を呼んだかどうか
	static inline bool isFinalize = false;
private://メンバ変数
	//オーディオ達
	std::map<std::string, std::unique_ptr<Audio>>audios_;
};

