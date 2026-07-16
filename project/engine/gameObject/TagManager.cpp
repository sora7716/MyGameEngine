#include "TagManager.h"
#include "Logger.h" 

//コンストラクタ
TagManager::TagManager(){
}

//デストラクタ
TagManager::~TagManager(){
}

//タグの追加
void TagManager::AddTag(const std::string& tag){
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
		return;
	}

	//削除する
	std::erase(tagList_, tag);
}

//タグを取得
std::string TagManager::GetTag() const{
	return ;
}

//タグ一覧を取得
std::vector<std::string> TagManager::GetTagList() const{
	return std::vector<std::string>();
}

//タグがListに存在しているか
bool TagManager::IsContainsTag(const std::string& tag){
	return std::find(tagList_.begin(), tagList_.end(), tag) != tagList_.end();
}
