#pragma once
#include "WinApi.h"
#include "DirectXBase.h"
#include "SRVManager.h"
#include "engine/input/Input.h"
#include "engine/2d/TextureManager.h"
#include "engine/3d/ModelManager.h"
#include "engine/debug/ImGuiManager.h"
#include "engine/camera/CameraManager.h"
#include "engine/2d/SpriteCommon.h"
#include "engine/2d/Object2dCommon.h"
#include "engine/3d/Object3dCommon.h"
#include "engine/debug/WireframeObject3dCommon.h"
#include "engine/particle/ParticleCommon.h"
#include "engine/scene/SceneManager.h"
#include "engine/audio/AudioManager.h"
#include "engine/particle/ParticleManager.h"
#include "GameObjectList.h"
#include "engine/scene/AbstractSceneFactory.h"
#include "engine/base/Context.h"
#include <memory>

/// <summary>
/// エンジンの核
/// </summary>
class Core {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Core() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Core() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// WinApiのゲッター
	/// </summary>
	/// <returns>WinApi</returns>
	WinApi* GetWinApi()const;

	/// <summary>
	/// DirectXの基盤部分
	/// </summary>
	/// <returns>DirectXの基盤部分</returns>
	DirectXBase* GetDirectXBase()const;

	/// <summary>
	/// SRVマネージャーのゲッター
	/// </summary>
	/// <returns>SRVマネージャー</returns>
	SRVManager* GetSRVManager()const;

	/// <summary>
	/// 入力のゲッター
	/// </summary>
	/// <returns>入力</returns>
	Input* GetInput()const;

	/// <summary>
	/// テクスチャマネージャーのゲッター
	/// </summary>
	/// <returns></returns>
	TextureManager* GetTextureManager()const;

	/// <summary>
	/// モデルマネージャーのゲッター
	/// </summary>
	/// <returns>モデルマネージャー</returns>
	ModelManager* GetModelManager()const;

	/// <summary>
	/// ImGuiマネージャーのゲッター
	/// </summary>
	/// <returns>ImGuiマネージャー</returns>
	ImGuiManager* GetImGuiManager()const;

	/// <summary>
	/// カメラマネージャーのゲッター
	/// </summary>
	/// <returns>カメラマネージャー</returns>
	CameraManager* GetCameraManager()const;

	/// <summary>
	/// スプライトの共通部分のゲッター
	/// </summary>
	/// <returns>スプライトの共通部分</returns>
	SpriteCommon* GetSpriteCommon()const;

	/// <summary>
	/// 2Dオブジェクトの共通部分のゲッター
	/// </summary>
	/// <returns>2Dオブジェクトの共通部分</returns>
	Object2dCommon* GetObject2dCommon()const;

	/// <summary>
	/// 3Dオブジェクトの共通部分のゲッター
	/// </summary>
	/// <returns>3Dオブジェクトの共通部分</returns>
	Object3dCommon* GetObject3dCommon()const;

	/// <summary>
	/// ワイヤーフレームオブジェクトの共通部分のゲッター
	/// </summary>
	/// <returns>ワイヤーフレームオブジェクト</returns>
	WireframeObject3dCommon* GetWireframeObject3dCommon()const;

	/// <summary>
	/// パーティクルの共通部分のゲッター
	/// </summary>
	/// <returns>パーティクルの共通部分</returns>
	ParticleCommon* GetParticleCommon()const;

	/// <summary>
	/// シーンマネージャーのゲッター
	/// </summary>
	/// <returns>シーンマネージャー</returns>
	SceneManager* GetSceneManager()const;

	/// <summary>
	/// オーディオマネージャーのゲッター
	/// </summary>
	/// <returns>オーディオマネージャー</returns>
	AudioManager* GetAudioManager()const;

	/// <summary>
	/// パーティクルのマネージャーのゲッター
	/// </summary>
	/// <returns>パーティクルマネージャー</returns>
	ParticleManager* GetParticleManager()const;

	/// <summary>
	/// ゲームオブジェクトのリストのゲッター
	/// </summary>
	/// <returns>ゲームオブジェクトのリストのゲッター</returns>
	GameObjectList* GetGameObjectList()const;

	/// <summary>
	/// シーンファクトリのゲッター
	/// </summary>
	/// <returns>シーンファクトリ</returns>
	AbstractSceneFactory* GetSceneFactory()const;
private://メンバ変数
	//WinApi
	std::unique_ptr<WinApi>winApi_ = nullptr;
	//DirectXの基盤部分
	std::unique_ptr<DirectXBase>directXBase_ = nullptr;
	//SRVマネージャー
	std::unique_ptr<SRVManager>srvManager_ = nullptr;
	//入力
	std::unique_ptr<Input>input_ = nullptr;
	//テクスチャマネージャー
	std::unique_ptr<TextureManager>textureManager_ = nullptr;
	//モデルマネージャー
	std::unique_ptr<ModelManager>modelManager_ = nullptr;
	//ImGuiマネージャー
	std::unique_ptr<ImGuiManager>imguiManager_ = nullptr;
	//カメラマネージャー
	std::unique_ptr<CameraManager>cameraManager_ = nullptr;
	//スプライトの共通部分
	std::unique_ptr<SpriteCommon>spriteCommon_ = nullptr;
	//2Dオブジェクトの共通部分
	std::unique_ptr<Object2dCommon>object2dCommon_ = nullptr;
	//3Dオブジェクトの共通部分
	std::unique_ptr<Object3dCommon>object3dCommon_ = nullptr;
	//ワイヤーフレームオブジェクトの共通部分
	std::unique_ptr<WireframeObject3dCommon>wireframeObject3dCommon_ = nullptr;
	//パーティクルの共通部分
	std::unique_ptr<ParticleCommon>particleCommon_ = nullptr;
	//シーンマネージャー
	std::unique_ptr<SceneManager>sceneManager_ = nullptr;
	//オーディオマネージャー
	std::unique_ptr<AudioManager>audioManager_ = nullptr;
	//パーティクルマネージャー
	std::unique_ptr<ParticleManager>particleManager_ = nullptr;
	//ゲームオブジェクトのリスト
	std::unique_ptr<GameObjectList>gameObjectList_ = nullptr;
	//シーンファクトリ
	std::unique_ptr< AbstractSceneFactory> sceneFactory_ = nullptr;
	//シーンで必要なもの
	SceneContext sceneContex_ = {};
};

