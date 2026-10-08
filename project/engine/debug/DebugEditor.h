#pragma once
#include "RenderingData.h"
#include "ImGuiManager.h"
#ifdef USE_IMGUI
#include "ImGuizmo.h"
#endif // USE_IMGUI
#include "Vector2.h"
#include "Vector3.h"
#include <vector>
#include <array>
#include <string>
#include <memory>
#include <optional>
#include <d3d12.h>

//前方宣言
class GameObject;
class TextureManager;
class TagManager;
class Camera;
class Object3d;

/// <summary>
/// タグの名前変更リクエスト用
/// </summary>
struct RenameTagRequest{
	std::string oldTag = "\0";
	std::string newTag = "\0";
};

#ifdef USE_IMGUI
//Gizmoのツール
enum class GizmoTool{
	kScale = ImGuizmo::SCALE,
	kRotate = ImGuizmo::ROTATE,
	kTranslate = ImGuizmo::TRANSLATE
};
#endif // USE_IMGUI

//親子付けのリクエスト
struct ParentRequest{
	GameObject* child = nullptr;
	GameObject* parent = nullptr;
	std::string nodePath = "";
};

/// <summary>
/// デバッグエディター
/// </summary>
class DebugEditor{
private://構造体
	/// <summary>
	/// シーンビューの矩形情報
	/// </summary>
	struct SceneViewRectInfo{
		Vector2 position = Vector2::GetZero();
		Vector2 size = Vector2::GetZero();
	};
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	DebugEditor();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~DebugEditor();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="textureManager">テクスチャの管理</param>
	/// <param name="tagManager">タグの管理</param>
	void Initialize(TextureManager* textureManager, TagManager* tagManager);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="sceneHandle">SceneのGPUハンドル</param>
	/// <param name="previewHandle">PreviewのGPUハンドル</param>
	void Draw(D3D12_GPU_DESCRIPTOR_HANDLE sceneHandle, D3D12_GPU_DESCRIPTOR_HANDLE previewHandle);

	/// <summary>
	/// ゲームオブジェクト一覧の設定
	/// </summary>
	/// <param name="gameObjects">ゲームオブジェクト一覧</param>
	void SetGameObjects(const std::vector<std::unique_ptr<GameObject>>& gameObjects);

	/// <summary>
	/// デバッグカメラの設定
	/// </summary>
	/// <param name="debugCamera">デバッグカメラ</param>
	void SetDebugCamera(Camera* debugCamera);

	/// <summary>
	/// 削除要求を取得
	/// </summary>
	/// <param name="target">対象</param>
	/// <returns>削除要求</returns>
	bool ConsumeDeleteRequest(GameObject*& target);

	/// <summary>
	/// 複製要求を取得
	/// </summary>
	/// <param name="target">対象</param>
	/// <returns>複製要求</returns>
	bool ConsumeDuplicateRequest(GameObject*& target);

	/// <summary>
	/// 生成要求を取得
	/// </summary>
	/// <returns>生成要求</returns>
	bool ConsumeCreateRequest();

	/// <summary>
	/// ゲームオブジェクトの移動要求を取得
	/// </summary>
	/// <param name="from">移動前の位置</param>
	/// <param name="to">移動後の位置</param>
	/// <returns>ゲームオブジェクトの移動要求</returns>
	bool ConsumeMoveGameObjectRequest(uint32_t& from, uint32_t& to);

	/// <summary>
	/// タグ名変更の要求の取得
	/// </summary>
	/// <param name="oldTag">前の名前</param>
	/// <param name="newTag">新し名前</param>
	/// <returns>新しい名前にするか</returns>
	bool ConsumeRenameTagRequest(std::string& oldTag, std::string& newTag);

	/// <summary>
	/// タグの削除の要求の取得
	/// </summary>
	/// <param name="tag">タグ</param>
	/// <returns>削除されたか</returns>
	bool ConsumeDeleteTagRequest(std::string& tag);

	/// <summary>
	/// 親子付けの要求の取得
	/// </summary>
	/// <param name="parentRequest">親子付けの要求</param>
	/// <returns>親子付けされたか</returns>
	bool ConsumeParentRequest(ParentRequest& parentRequest);

	/// <summary>
	/// 親子付け解除の要求を取得
	/// </summary>
	/// <param name="target">対象</param>
	/// <returns>親子付け解除されたか</returns>
	bool ConsumeDetachRequest(GameObject*& target);

	/// <summary>
	/// GameObjectを選択
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	void SelectGameObject(GameObject* gameObject);

	/// <summary>
	/// シーンのImGuiウィンドウを選択しているかの取得
	/// </summary>
	/// <returns>シーンのImGuiウィンドウを選択しているか</returns>
	bool IsSceneViewHovered()const;

	/// <summary>
	/// プレビューのImGuiウィンドウを選択しているかの取得
	/// </summary>
	/// <returns>プレビューのImGuiウィンドウを選択しているか</returns>
	bool IsPreviewHovered()const;
private://メンバ関数
	/// <summary>
	/// ドッキングスペースの描画
	/// </summary>
	void DrawDockSpace();

	/// <summary>
	/// ヒエラルキーの描画
	/// </summary>
	void DrawHierarchy();

	/// <summary>
	/// シーンの描画
	/// </summary>
	/// <param name="handle">SceneのGPUハンドル</param>
	void DrawScene(D3D12_GPU_DESCRIPTOR_HANDLE handle);

	/// <summary>
	/// Gizmo切り替え用ツールバーの描画
	/// </summary>
	void DrawGizmoToolbar();

	/// <summary>
	/// Gizmoの描画
	/// </summary>
	void DrawGizmo();

	/// <summary>
	/// プレビューシーンの描画
	/// </summary>
	/// <param name="handle">PreviewSceneのGPUハンドル</param>
	void DrawPreview(D3D12_GPU_DESCRIPTOR_HANDLE handle);

	/// <summary>
	/// インスペクターの描画
	/// </summary>
	void DrawInspector();

	/// <summary>
	/// タグの管理の描画
	/// </summary>
	void DrawTagManager();

	/// <summary>
	/// 名前変更を開始
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	void BeginRename(GameObject* gameObject);

	/// <summary>
	/// ノードのツリーを描画
	/// </summary>
	/// <param name="node">ノード</param>
	/// <param name="parentPath">親のパス</param>
	/// <param name="targetObject">対象となるObject3d</param>
	void DrawNodeTree(const Node& node, const std::string& parentPath, Object3d* targetObject);
private://定数
	//ゲームオブジェクトをDragAndDropで動かす用のpayloadType
	static inline const std::string kGameObjectPayloadType = "GameObjectPayload";
private://メンバ変数
	//テクスチャの管理
	TextureManager* textureManager_ = nullptr;
	//タグの管理
	TagManager* tagManager_ = nullptr;

	//テクスチャを読み込む際のディレクトリパス
	std::string directoryPath = "engine/resources/editorIcons/";

	//選択するゲームオブジェクト
	GameObject* selectedGameObject_ = nullptr;
	//オブジェクトの詳細で編集するオイラー角
	Vector3 inspectorEulerAngle_ = Vector3::GetZero();

	//ゲームオブジェクトの一覧へのポインタ
	const std::vector<std::unique_ptr<GameObject>>* gameObjects_ = nullptr;

	//作成要求
	bool requestCreateGameObject_ = false;
	//複製要求
	GameObject* requestDuplicateGameObject_ = nullptr;
	//削除要求
	GameObject* requestDeleteGameObject_ = nullptr;

	//名前変更
	GameObject* renamingGameObject_ = nullptr;
	//名前変更用の文字列バッファ
	std::array<char, 256>renameObjectBuffer_ = {};
	//InputTextへフォーカスする要求
	bool requestRenameFocus_ = false;

	//タグの管理を開くかのフラグ
	bool isTegManagerOpen_ = false;
	//新規タグ入力用
	std::array<char, 128>newTagNameBuffer_{};
	//名前変更用
	std::array<char, 128>renameTagBuffer_{};
	std::string renameTargetTag_ = "\0";
	//タグ名変更の要求
	std::optional<RenameTagRequest>requestRenameTag_ = {};
	//タグ削除の要求
	std::optional<std::string>requestDeleteTag_ = "\0";
	//削除用
	std::string deleteTargetTag_ = "\0";

	//移動前のインデックス
	uint32_t draggedIndex_ = 0;
	//移動後のインデックス
	uint32_t dropTargetIndex_ = 0;
	//ゲームオブジェクトの移動要求
	bool requestMoveGameObject_ = false;

	//シーンビューを選択しているか
	bool isSceneViewHovered_ = false;
	//プレビューを選択しているか
	bool isPreviewHovered_ = false;

	//シーンビューの矩形情報
	SceneViewRectInfo sceneViewRectInfo_ = {};

	//デバッグカメラ
	Camera* debugCamera_ = nullptr;

#ifdef USE_IMGUI
	//Gizmoツール
	GizmoTool gizmoTool_ = GizmoTool::kTranslate;
#endif // USE_IMGUI
	//Gizmoツールバーを表示するか
	bool isGizmoToolbarVisible_ = true;

	//選択中のNodeパス
	std::string selectedNodePath_ = "";

	//親子付けの要求
	std::optional<ParentRequest>requestAttachTo_ = {};

	//親子付け解除の要求
	std::optional<GameObject*>requestDetach_ = {};
};

