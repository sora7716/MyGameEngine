#pragma once
#include "engine/audio/AudioManager.h"
#include "engine/3d/ModelManager.h"

//前方宣言
class Core;

/// <summary>
/// ゲームで使用するオブジェクトの読み込みのリスト
/// </summary>
class GameObjectList{
public://メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameObjectList();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="core">エンジンの核</param>
	void Initialize(Core*core);
public://PrassKey
	class ConstructorKey {
		ConstructorKey() = default;
		friend class Core;
	};
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PrassKeyを受け取る</param>
	explicit GameObjectList(ConstructorKey);
private://メンバ関数
	//コピーコンストラクタ禁止
	GameObjectList(const GameObjectList&) = delete;
	//代入演算子の禁止
	GameObjectList operator=(const GameObjectList&) = delete;

	/// <summary>
	/// オーディオの読み込み
	/// </summary>
	void LoadAudio();

	/// <summary>
	/// OBJファイルの読み込み
	/// </summary>
	void LoadModel();

	/// <summary>
	/// カメラの生成
	/// </summary>
	void CreateCamera();
private://静的メンバ変数
	//インスタンス
	static inline GameObjectList* instance = nullptr;
	//Finalizeをしたかどうか
	static inline bool isFinalize = false;
	//エンジンの核
	Core* core_ = nullptr;
};

