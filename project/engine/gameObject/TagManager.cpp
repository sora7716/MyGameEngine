#include "TagManager.h"
#include "StringUtility.h"
#include "Logger.h"
#include <format>

//デストラクタ
TagManager::~TagManager(){
}

//初期化
void TagManager::Initialize(){
	//タグリストのクリア
	tagList_.clear();
	//デフォルトタグの追加
	tagList_.push_back(kDefaultTagName);
}

//タグの追加
void TagManager::AddTag(const std::string& tag){
	//空白じゃないか
	if (tag.empty()){
		return;
	}

	//存在しているか
	if (IsContainsTag(tag)){
		return;
	}

	//存在していない場合
	tagList_.push_back(tag);
}

//タグの削除
void TagManager::RemoveTag(const std::string& tag){
	//存在しているか
	if (!IsContainsTag(tag)){
		logger::ConsolePrintf("指定したTagはそもそも存在していません");
		return;
	}

	//UnTaggedかどうか
	if (tag == kDefaultTagName){
		logger::ConsolePrintf(std::format(L"{}は削除できません", stringUtility::ConvertString(kDefaultTagName)));
		return;
	}

	//削除する
	std::erase(tagList_, tag);
}

//タグの名前変更
void TagManager::RenameTag(const std::string& tag, const std::string& newTagName){
	//リストに存在しているか
	if (!IsContainsTag(tag)){
		logger::ConsolePrintf("指定したTagはそもそも存在していません");
		return;
	}

	//UnTaggedかどうか
	if (tag == kDefaultTagName){
		logger::ConsolePrintf(std::format(L"{}は削除できません", stringUtility::ConvertString(kDefaultTagName)));
		return;
	}

	//新しい名前が今までにある名前になっていないか
	if (IsContainsTag(newTagName)){
		logger::ConsolePrintf("すでにあるタグと同じ名前は設定できません");
		return;
	}

	//名前変更
	std::vector<std::string>::iterator it = std::find(tagList_.begin(), tagList_.end(), tag);
	*it = newTagName;
}

//タグの取得
std::string TagManager::GetTag(uint32_t tagIndex) const{
	if (tagIndex >= tagList_.size()){
		logger::ConsolePrintf("tagIndexがtagListの要素数を超えています");
		return kDefaultTagName;
	}
	return tagList_[tagIndex];
}

//タグ一覧を取得
const std::vector<std::string>& TagManager::GetTagList() const{
	return tagList_;
}

//タグがListに存在しているか
bool TagManager::IsContainsTag(const std::string& tag){
	return std::find(tagList_.begin(), tagList_.end(), tag) != tagList_.end();
}

//コンストラクタ
TagManager::TagManager(ConstructorKey){
}
