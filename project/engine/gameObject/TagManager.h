#pragma once
#include <string>
#include <vector>

/// <summary>
/// タグの管理
/// </summary>
class TagManager{
public://メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~TagManager();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// タグの追加
	/// </summary>
	/// <param name="tag">タグ</param>
	void AddTag(const std::string& tag);

	/// <summary>
	/// タグの削除
	/// </summary>
	/// <param name="tag">タグ</param>
	void RemoveTag(const std::string& tag);

	/// <summary>
	/// タグの名前変更
	/// </summary>
	/// <param name="tag">タグ</param>
	/// <param name="newTagName">新しいタグ名</param>
	void RenameTag(const std::string& tag, const std::string& newTagName);

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
public://PassKey
	class ConstructorKey{
	private:
		ConstructorKey() = default;
		friend class Core;
	};

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PassKeyを受け取る</param>
	explicit TagManager(ConstructorKey);
public://定数
	//デフォルトで存在するタグ
	static inline const std::string kDefaultTagName = "UnTagged";
private://メンバ関数
	/// <summary>
	/// タグがListに存在しているか
	/// </summary>
	/// <param name="tag">タグ</param>
	/// <returns>存在しているかのフラグ</returns>
	bool IsContainsTag(const std::string& tag);
private://メンバ変数
	std::vector<std::string>tagList_;
};
