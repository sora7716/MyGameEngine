#pragma once
#include <string>
#include <vector>
#include <memory>

/// <summary>
/// タグの管理
/// </summary>
class TagManager{
public://PassKey
	class ConstructorKey{
	private:
		ConstructorKey() = default;
		friend class Core;
	};
public://静的メンバ関数
	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="key">コンストラクタのKey</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<TagManager>Create(ConstructorKey key);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PassKeyを受け取る</param>
	explicit TagManager(ConstructorKey);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TagManager();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// タグがリストの中にあるか
	/// </summary>
	/// <param name="tag">タグ</param>
	/// <returns>タグがリストにあるか</returns>
	bool IsContainsTag(const std::string& tag);

	/// <summary>
	/// タグの追加
	/// </summary>
	/// <param name="tag">タグ</param>
	void AddTag(const std::string& tag);

	/// <summary>
    /// タグの削除
    /// </summary>
    /// <param name="tag">タグ</param>
	/// <returns>タグの削除ができたかどうか</returns>
	bool RemoveTag(const std::string& tag);

	/// <summary>
    /// タグの名前変更
    /// </summary>
    /// <param name="tag">タグ</param>
    /// <param name="newTagName">新しいタグ名</param>
	/// <returns>タグの名前変更ができたかどうか</returns>
	bool RenameTag(const std::string& tag, const std::string& newTagName);

	/// <summary>
	/// タグの取得
	/// </summary>
	/// <param name="tagIndex">タグの検索キー</param>
	/// <returns>タグ</returns>
	std::string GetTag(uint32_t tagIndex)const;

	/// <summary>
	/// タグ一覧の取得
	/// </summary>
	/// <returns>タグ一覧</returns>
	const std::vector<std::string>& GetTagList()const;
public://定数
	//デフォルトで存在するタグ
	static inline const std::string kDefaultTagName = "UnTagged";
private://メンバ関数
	//コピーコンストラクタ禁止
	TagManager(const TagManager&) = delete;
	//代入演算子の禁止
	TagManager operator=(const TagManager&) = delete;

	/// <summary>
	/// タグがListに存在しているか
	/// </summary>
	/// <param name="tagList">タグリスト</param>
	/// <param name="tag">タグ</param>
	/// <returns>存在しているかのフラグ</returns>
	bool IsContainsTag(const std::vector<std::string>& tagList, const std::string& tag);

	/// <summary>
	/// タグリストの保存
	/// </summary>
	void SeveTagList();

	/// <summary>
	/// タグリストの読み込み
	/// </summary>
	void LoadTagList();
private://メンバ変数
	std::vector<std::string>tagList_;
};
