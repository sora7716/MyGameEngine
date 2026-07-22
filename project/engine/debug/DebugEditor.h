#pragma once
#include <vector>
#include <array>
#include <string>
#include <memory>
#include <optional>
//前方宣言
class GameObject;
class TagManager;

/// <summary>
/// タグの名前変更リクエスト用
/// </summary>
struct RenameTagRequest{
	std::string oldTag;
	std::string newTag;
};

/// <summary>
/// デバッグエディター
/// </summary>
class DebugEditor{
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
	/// <param name="tagManager">タグの管理</param>
	void Initialize(TagManager* tagManager);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// ゲームオブジェクト一覧の設定
	/// </summary>
	/// <param name="gameObjects">ゲームオブジェクト一覧</param>
	void SetGameObjects(const std::vector<std::unique_ptr<GameObject>>& gameObjects);

	/// <summary>
	/// 削除要求を取得
	/// </summary>
	/// <returns>削除要求</returns>
	GameObject* ConsumeDeleteRequest();

	/// <summary>
	/// 複製要求を取得
	/// </summary>
	/// <returns>複製要求</returns>
	GameObject* ConsumeDuplicateRequest();

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
	bool ConsumeMoveGameObjectRequest(uint32_t& from,uint32_t& to);

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
	/// GameObjectを選択
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	void SelectGameObject(GameObject* gameObject);
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
private://定数
	//ゲームオブジェクトのpayloadType
	static inline const std::string kGameObjectPayloadType = "GameObjectPayload";
private://メンバ変数
	//選択するゲームオブジェクト
	GameObject* selectedGameObject_ = nullptr;
	//ゲームオブジェクトの一覧へのポインタ
	const std::vector<std::unique_ptr<GameObject>>* gameObjects_;
	//作成要求
	bool requestCreateGameObject_ = false;
	//複製要求
	GameObject* requestDuplicateGameObject_ = nullptr;
	//削除要求
	GameObject* requestDeleteGameObject_ = nullptr;
	//名前変更
	GameObject* renamingGameObject_ = nullptr;
	//名前変更用の文字列バッファ
	std::array<char, 256>renameBuffer_ = {};
	//InputTextへフォーカスする要求
	bool requestRenameFocus_ = false;
	//タグの管理
	TagManager* tagManager_ = nullptr;
	//タグの管理を開くかのフラグ
	bool isTegManagerOpen_ = false;
	//新規タグ入力用
	std::array<char, 128>newTagNameBuffer_{};
	//名前変更用
	std::array<char, 128>renameBuffer__{};
	std::string renameTargetTag_;
	//タグ名変更の要求
	std::optional<RenameTagRequest>requestRenameTag_;
	//タグ削除の要求
	std::optional<std::string>requestDeleteTag_;
	//削除用
	std::string deleteTargetTag_;
	//移動前のインデックス
	uint32_t draggedIndex_ = 0;
	//移動後のインデックス
	uint32_t dropTargetIndex_ = 0;
	//ゲームオブジェクトの移動要求
	bool requestMoveGameObject_ = false;
};

