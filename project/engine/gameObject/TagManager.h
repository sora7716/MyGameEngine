#pragma once
#include <string>
#include <vector>

/// <summary>
/// タグの管理
/// </summary>
class TagManager{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	TagManager();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TagManager();

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
	/// タグの取得
	/// </summary>
	/// <returns>タグ</returns>
	std::string GetTag()const;

	/// <summary>
	/// タグ一覧の取得
	/// </summary>
	/// <returns>タグ一覧</returns>
	std::vector<std::string>GetTagList()const;
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
	std::vector<std::string>tagList_ = { kDefaultTagName };
};
