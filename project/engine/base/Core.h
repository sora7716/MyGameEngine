#pragma once
#include "Context.h"
#include <memory>

//前方宣言
class WinApi;
class DirectXBase;
class SRVManager;
class Input;
class TextureManager;
class ModelManager;
class ImGuiManager;
class SceneManager;
class AudioManager;
class DefaultResourceLoader;
class AbstractSceneFactory;
class TagManager;
class PipelineManager;
class LightingManager;
class RenderSystem;

/// <summary>
/// エンジンの核
/// </summary>
class Core{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Core();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Core();

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
	/// ゲームオブジェクトのリストの取得
	/// </summary>
	/// <returns>ゲームオブジェクトのリストの取得</returns>
	DefaultResourceLoader* GetGameObjectList()const;

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

	/// <summary>
	/// ライティングの管理の取得
	/// </summary>
	/// <returns>ライティングの管理</returns>
	LightingManager* GetLightingManager()const;

	/// <summary>
	/// 描画システムの取得
	/// </summary>
	/// <returns>描画システム</returns>
	RenderSystem* GetRenderSystem()const;
private://メンバ関数
	//コピーコンストラクタ禁止
	Core(const Core&) = delete;
	//代入演算子の禁止
	Core operator=(const Core&) = delete;
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
	std::unique_ptr<ImGuiManager>imGuiManager_ = nullptr;
	//シーンマネージャー
	std::unique_ptr<SceneManager>sceneManager_ = nullptr;
	//オーディオマネージャー
	std::unique_ptr<AudioManager>audioManager_ = nullptr;
	//ゲームオブジェクトのリスト
	std::unique_ptr<DefaultResourceLoader>gameObjectList_ = nullptr;
	//シーンファクトリ
	std::unique_ptr< AbstractSceneFactory> sceneFactory_ = nullptr;
	//タグの管理
	std::unique_ptr<TagManager>tagManager_ = nullptr;
	//パイプラインの管理
	std::unique_ptr<PipelineManager>pipelineManager_ = nullptr;
	//ライティングの管理
	std::unique_ptr<LightingManager>lightingManager_ = nullptr;
	//描画システム
	std::unique_ptr<RenderSystem>renderSystem_ = nullptr;
	//シーンで必要なもの
	SceneContext sceneContext_ = {};
};

