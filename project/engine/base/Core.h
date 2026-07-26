#pragma once
#include "WinApi.h"
#include "DirectXBase.h"
#include "SRVManager.h"
#include "Input.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "CameraManager.h"
#include "SpriteCommon.h"
#include "Object3dCommon.h"
#include "ParticleCommon.h"
#include "ModelCommon.h"
#include "SceneManager.h"
#include "AudioManager.h"
#include "ParticleManager.h"
#include "GameObjectList.h"
#include "AbstractSceneFactory.h"
#include "TagManager.h"
#include "PipelineManager.h"
#include "Context.h"
#include <memory>

/// <summary>
/// エンジンの核
/// </summary>
class Core{
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
	/// WinApiの取得
	/// </summary>
	/// <returns>WinApi</returns>
	WinApi* GetWinApi()const;

	/// <summary>
	/// DirectXの基盤部分
	/// </summary>
	/// <returns>DirectXの基盤部分</returns>
	DirectXBase* GetDirectXBase()const;

	/// <summary>
	/// SRVマネージャーの取得
	/// </summary>
	/// <returns>SRVマネージャー</returns>
	SRVManager* GetSRVManager()const;

	/// <summary>
	/// 入力の取得
	/// </summary>
	/// <returns>入力</returns>
	Input* GetInput()const;

	/// <summary>
	/// テクスチャマネージャーの取得
	/// </summary>
	/// <returns></returns>
	TextureManager* GetTextureManager()const;

	/// <summary>
	/// モデルマネージャーの取得
	/// </summary>
	/// <returns>モデルマネージャー</returns>
	ModelManager* GetModelManager()const;

	/// <summary>
	/// ImGuiマネージャーの取得
	/// </summary>
	/// <returns>ImGuiマネージャー</returns>
	ImGuiManager* GetImGuiManager()const;

	/// <summary>
	/// カメラマネージャーの取得
	/// </summary>
	/// <returns>カメラマネージャー</returns>
	CameraManager* GetCameraManager()const;

	/// <summary>
	/// スプライトの共通部分の取得
	/// </summary>
	/// <returns>スプライトの共通部分</returns>
	SpriteCommon* GetSpriteCommon()const;

	/// <summary>
	/// 3Dオブジェクトの共通部分の取得
	/// </summary>
	/// <returns>3Dオブジェクトの共通部分</returns>
	Object3dCommon* GetObject3dCommon()const;

	/// <summary>
	/// パーティクルの共通部分の取得
	/// </summary>
	/// <returns>パーティクルの共通部分</returns>
	ParticleCommon* GetParticleCommon()const;

	/// <summary>
	/// モデルの共通部分の取得
	/// </summary>
	/// <returns>モデルの共通部分</returns>
	ModelCommon* GetModelCommon()const;

	/// <summary>
	/// シーンマネージャーの取得
	/// </summary>
	/// <returns>シーンマネージャー</returns>
	SceneManager* GetSceneManager()const;

	/// <summary>
	/// オーディオマネージャーの取得
	/// </summary>
	/// <returns>オーディオマネージャー</returns>
	AudioManager* GetAudioManager()const;

	/// <summary>
	/// パーティクルのマネージャーの取得
	/// </summary>
	/// <returns>パーティクルマネージャー</returns>
	ParticleManager* GetParticleManager()const;

	/// <summary>
	/// ゲームオブジェクトのリストの取得
	/// </summary>
	/// <returns>ゲームオブジェクトのリストの取得</returns>
	GameObjectList* GetGameObjectList()const;

	/// <summary>
	/// シーンファクトリの取得
	/// </summary>
	/// <returns>シーンファクトリ</returns>
	AbstractSceneFactory* GetSceneFactory()const;

	/// <summary>
	/// タグの管理の取得
	/// </summary>
	/// <returns>タグの管理</returns>
	TagManager* GetTagManager()const;

	/// <summary>
	/// パイプラインの管理の取得
	/// </summary>
	/// <returns>パイプラインの管理</returns>
	PipelineManager* GetPipelineManager()const;
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
	//3Dオブジェクトの共通部分
	std::unique_ptr<Object3dCommon>object3dCommon_ = nullptr;
	//パーティクルの共通部分
	std::unique_ptr<ParticleCommon>particleCommon_ = nullptr;
	//モデルの共通部分
	std::unique_ptr<ModelCommon> modelCommon_ = nullptr;
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
	//タグの管理
	std::unique_ptr<TagManager>tagManager_ = nullptr;
	//パイプラインの管理
	std::unique_ptr<PipelineManager>pipelineManager_ = nullptr;
	//シーンで必要なもの
	SceneContext sceneContext_ = {};
};

