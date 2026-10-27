#pragma once
#include "Vector2.h"
#include "ImGuiManager.h"
#ifdef USE_IMGUI
#include <ImGuizmo.h>
#endif // USE_IMGUI
#include <string>
#include <memory>

//前方宣言
class Camera;
class GameObject;
class TextureManager;

#ifdef USE_IMGUI
//Gizmoのツール
enum class GizmoTool{
	kScale = ImGuizmo::SCALE,
	kRotate = ImGuizmo::ROTATE,
	kTranslate = ImGuizmo::TRANSLATE
};
#endif // USE_IMGUI

/// <summary>
/// Gizmoシステム
/// </summary>
class GizmoSystem{
public://静的メンバ関数
	/// <summary>
	/// 生成
	/// </summary>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<GizmoSystem>Create();
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GizmoSystem();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GizmoSystem();

	/// <summary>
	/// ツールバーの描画
	/// </summary>
	/// <param name="textureManager">テクスチャの管理</param>
	void DrawToolbar(TextureManager*textureManager);

	/// <summary>
	/// 選択中のオブジェクトの Transform を操作する
	/// </summary>
	/// <param name="sceneViewPosition">シーンビューの位置</param>
	/// <param name="sceneViewSize">シーンビューのサイズ</param>
	/// <returns>成功したか</returns>
	bool ManipulateSelectedTransform(const Vector2& sceneViewPosition, const Vector2& sceneViewSize);

	/// <summary>
	/// デバッグカメラの設定
	/// </summary>
	/// <param name="debugCamera">デバッグカメラ</param>
	void SetDebugCamera(Camera*debugCamera);

	/// <summary>
	/// 選択されたゲームオブジェクトを設定
	/// </summary>
	/// <param name="selectedGameObject">選択されたゲームオブジェクト</param>
	void SetSelectedGameObject(GameObject* selectedGameObject);

	/// <summary>
	/// 選択したノードパスを設定
	/// </summary>
	/// <param name="selectedNodePath">選択したノードパス</param>
	void SetSelectedNodePath(const std::string& selectedNodePath);

	/// <summary>
	/// Gizmoツールバーを表示するかの取得
	/// </summary>
	/// <returns>Gizmoツールバーを表示するか</returns>
	bool& IsToolbarVisible();
private://メンバ変数
	//選択したオブジェクト
	GameObject* selectedGameObject_ = nullptr;
	//デバッグカメラ
	Camera* debugCamera_ = nullptr;
	//選択したノードパス
	std::string selectedNodePath_ = "";
	
	//テクスチャを読み込む際のディレクトリパス
	std::string directoryPath_ = "engine/resources/editorIcons/";
#ifdef USE_IMGUI
	//Gizmoツール
	GizmoTool gizmoTool_ = GizmoTool::kTranslate;
#endif // USE_IMGUI
	//Gizmoツールバーを表示するか
	bool isToolbarVisible_ = true;
};

