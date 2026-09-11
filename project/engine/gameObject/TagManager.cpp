#include "TagManager.h"
#include "StringUtility.h"
#include "Logger.h"
#include <format>
#include <nlohmann/json.hpp>
#include <fstream>

//生成
std::unique_ptr<TagManager> TagManager::Create(ConstructorKey key){
	//生成
	std::unique_ptr<TagManager>instance = std::make_unique<TagManager>(key);
	//初期化
	instance->Initialize();

	return instance;
}

//コンストラクタ
TagManager::TagManager(ConstructorKey){
}

//デストラクタ
TagManager::~TagManager(){
}

//初期化
void TagManager::Initialize(){
	//タグリストのクリア
	tagList_.clear();
	//デフォルトタグの追加
	tagList_.push_back(kDefaultTagName);
	//タグの読み込み
	LoadTagList();
}

//タグがリストにあるか
bool TagManager::IsContainsTag(const std::string& tag){
	return IsContainsTag(tagList_, tag);
}

//タグの追加
void TagManager::AddTag(const std::string& tag){
	//空白じゃないか
	if (tag.empty()){
		return;
	}

	//存在しているか
	if (IsContainsTag(tagList_, tag)){
		return;
	}

	//存在していない場合
	tagList_.push_back(tag);

	//タグの保存
	SeveTagList();
}

//タグの削除
bool TagManager::RemoveTag(const std::string& tag){
	//存在しているか
	if (!IsContainsTag(tag)){
		Logger::OutputLog("指定したTagはそもそも存在していません");
		return false;
	}

	//UnTaggedかどうか
	if (tag == kDefaultTagName){
		Logger::OutputLog(std::format(L"{}は削除できません", stringUtility::ConvertString(kDefaultTagName)));
		return false;
	}

	//削除する
	std::erase(tagList_, tag);

	//タグの保存
	SeveTagList();

	return true;
}

//タグの名前変更
bool TagManager::RenameTag(const std::string& tag, const std::string& newTagName){
	//リストに存在しているか
	if (!IsContainsTag(tag)){
		Logger::OutputLog("指定したTagはそもそも存在していません");
		return false;
	}

	//UnTaggedかどうか
	if (tag == kDefaultTagName){
		//Logger::OutputLog(std::format(L"{}は削除できません", kDefaultTagName));
		return false;
	}

	//新しい名前が今までにある名前になっていないか
	if (IsContainsTag(newTagName)){
		Logger::OutputLog("すでにあるタグと同じ名前は設定できません");
		return false;
	}

	//名前変更
	std::vector<std::string>::iterator it = std::find(tagList_.begin(), tagList_.end(), tag);
	*it = newTagName;

	//タグの保存
	SeveTagList();

	return true;
}

//タグの取得
std::string TagManager::GetTag(uint32_t tagIndex) const{
	if (tagIndex >= tagList_.size()){
		Logger::OutputLog("tagIndexがtagListの要素数を超えています");
		return kDefaultTagName;
	}
	return tagList_[tagIndex];
}

//タグ一覧を取得
const std::vector<std::string>& TagManager::GetTagList() const{
	return tagList_;
}

//タグがListに存在しているか
bool TagManager::IsContainsTag(const std::vector<std::string>& tagList, const std::string& tag){
	return std::find(tagList.begin(), tagList.end(), tag) != tagList.end();
}

//タグリストの保存
void TagManager::SeveTagList(){
	nlohmann::json jsonData;
	jsonData["tags"] = tagList_;

	//Jsonファイルを開く
	std::ofstream outputFile("TagManager.json");

	//ファイルが開けるか確認
	if (!outputFile.is_open()){
		Logger::OutputLog("TagManager.jsonが開けませんでした");
	}

	//Jsonに書き込み
	outputFile << jsonData.dump(4);
}

//タグリストの読み込み
void TagManager::LoadTagList(){
	//ファイルを開く
	std::ifstream inputFile("TagManager.json");

	//ファイルが開けるか確認
	if (!inputFile.is_open()){
		//ファイルが開けないのでセーブを行う
		SeveTagList();
		return;
	}

	nlohmann::json jsonData;
	//Jsonデータに読み込んだ内容を書き込む
	inputFile >> jsonData;
	//tagsという項目があるかを確認
	if (!jsonData.contains("tags")){
		Logger::OutputLog("tagsという項目は見つかりませんでした");
		return;
	}

	//配列になっているか確認
	if (!jsonData["tags"].is_array()){
		Logger::OutputLog("tagsが配列になっていませんでした");
		return;
	}

	//一時的に保存用の配列
	std::vector<std::string>tempTagList;

	//先頭にデフォルトだぐを追加
	tempTagList.push_back(kDefaultTagName);

	//空白、デフォルトタグ、重複を検査
	for (const std::string& tag : jsonData["tags"].get<std::vector<std::string>>()){
		//空白があったら
		if (tag.empty()){
			Logger::OutputLog("tagが空白なのでListから省きます");
			continue;
		}

		//デフォルトタグがあった場合
		if (tag == kDefaultTagName){
			//Logger::OutputLog(std::format("{}があるのでList追加からは省きます", stringUtility::ConvertString(kDefaultTagName)));
			continue;
		}

		//一時保存用の配列の中にtagがあるか
		if (IsContainsTag(tempTagList, tag)){
			continue;
		}

		//タグを追加
		tempTagList.push_back(tag);
	}

	//タグリストに反映
	tagList_ = tempTagList;
}