#include "GlobalVariables.h"
#include "ImGuiManager.h"
#include <fstream>

//インスタンスのゲッター
GlobalVariables* GlobalVariables::GetInstance() {
	assert(!isFinalize && "GetInstance() called after Finalize()");
	if (instance == nullptr) {
		instance = new GlobalVariables();
	}
	return instance;
}

//グループの生成
void GlobalVariables::CreateGroup(const std::string& groupName) {
	//読み込み済みならカメラを検索
	if (dataList_.contains(groupName)) {
		//読み込み済みなら早期return
		return;
	}

	//グループの生成
	Group group = {};

	//カメラをmapコンテナに格納する
	dataList_.insert(std::make_pair(groupName, std::move(group)));
}

//更新
void GlobalVariables::Update() {
#ifdef USE_IMGUI
	//読み込みフラグをオフ
	isLoaded_ = false;

	if (!ImGui::Begin("Global Variables", nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}

	if (!ImGui::BeginMenuBar()) {
		return;
	}

	//各グループについて
	for (std::map<std::string, Group>::iterator itGroup = dataList_.begin(); itGroup != dataList_.end(); itGroup++) {
		//グループ名を取得
		const std::string& groupName = itGroup->first;
		//グループの参照を取得
		Group& group = itGroup->second;

		if (!ImGui::BeginMenu(groupName.c_str())) {
			continue;
		}

		//各項目について
		for (std::map<std::string, Item>::iterator itItem = group.begin(); itItem != group.end(); itItem++) {

			//項目名を取得
			const std::string& itemName = itItem->first;
			//項目の参照を取得
			Item& item = itItem->second;

			//項目の方によって処理を変える
			if (std::holds_alternative<int32_t>(item)) {
				//int32_t型の値を保持してれば
				int32_t* ptr = std::get_if<int32_t>(&item);
				ImGuiManager::CheckBoxToInt(itemName, *ptr);
			} else if (std::holds_alternative<float>(item)) {
				//float型の値を保持してれば
				float* ptr = std::get_if<float>(&item);
				ImGui::DragFloat(itemName.c_str(), ptr, 0.1f);
			} else if (std::holds_alternative<Vector3>(item)) {
				//Vector3型の値を保持してれば
				Vector3* ptr = std::get_if<Vector3>(&item);
				ImGui::DragFloat3(itemName.c_str(), reinterpret_cast<float*>(ptr), 0.1f);
			} else if (std::holds_alternative<TransformData>(item)) {
				//TransformData型の値を保持してれば
				TransformData* ptr = std::get_if<TransformData>(&item);
				ImGuiManager::DragTransform(*ptr);
			} else if (std::holds_alternative<Vector4>(item)) {
				//Vector4型の値を保持してれば
				Vector4* ptr = std::get_if<Vector4>(&item);
				ImGui::ColorEdit4(itemName.c_str(), reinterpret_cast<float*>(ptr));
			}
		}

		//改行
		ImGui::Text("\n");

		//ファイルの保存ボタン
		if (ImGui::Button("save")) {
			SaveFile(groupName);
			std::string message = std::format("{}.json saved.", groupName);
			MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		}

		//改行
		ImGui::Text("\n");

		//ファイルの読み込みボタン
		if (ImGui::Button("load")) {
			LoadFile(groupName);
			std::string message = std::format("{}.json loaded.", groupName);
			MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		}

		ImGui::EndMenu();
	}

	ImGui::EndMenuBar();
	ImGui::End();
#endif // USE_IMGUI
}

//終了
void GlobalVariables::Finalize() {
	delete instance;
	instance = nullptr;
	isFinalize = true;
}

//読み込みをしたかのフラグのゲッター
bool GlobalVariables::IsLoaded()const {
	return isLoaded_;
}

//ディレクトリの全ファイル読み込み
void GlobalVariables::LoadFiles() {
	//保存先のディレクトリのパスをローカル変数に宣言する
	std::string directoryPath = kDirectoryPath;
	//ディレクトリがなければスキップ
	if (!std::filesystem::exists(kDirectoryPath)) {
		return;
	}

	std::filesystem::directory_iterator dir_it(directoryPath);
	for (const std::filesystem::directory_entry& entry : dir_it) {
		//ファイルパスを取得
		const std::filesystem::path& filePath = entry.path();

		//ファイルの拡張子を取得
		std::string extension = filePath.extension().string();

		//.jsonファイル以外はスキップ
		if (extension.compare(".json") != 0) {
			continue;
		}

		//ファイルの読み込み
		LoadFile(filePath.stem().string());
	}
}

//設定ファイルの書き出し
void GlobalVariables::SaveFile(const std::string& groupName) {
	std::map<std::string, Group>::iterator itGroup = dataList_.find(groupName);

	//未登録チェック
	assert(itGroup != dataList_.end());

	json root;

	root = json::object();

	//jsonオブジェクトを登録
	root[groupName] = json::object();

	//各項目について
	for (std::map<std::string, Item>::iterator itItem = itGroup->second.begin(); itItem != itGroup->second.end(); itItem++) {
		//項目名を取得
		const std::string& itemName = itItem->first;
		//項目の参照を取得
		Item& item = itItem->second;
		//項目の方によって処理を変える
		if (std::holds_alternative<int32_t>(item)) {
			//int32_t型の値を登録
			root[groupName][itemName] = std::get<int32_t>(item);
		} else if (std::holds_alternative<float>(item)) {
			//float型の値を登録
			root[groupName][itemName] = std::get<float>(item);
		} else if (std::holds_alternative<Vector3>(item)) {
			//float型のjson配列を登録
			Vector3 value = std::get<Vector3>(item);
			root[groupName][itemName] = json::array({ value.x,value.y,value.z });
		} else if (std::holds_alternative<TransformData>(item)) {
			//float型のjson配列を登録
			TransformData value = std::get<TransformData>(item);
			root[groupName][itemName] = json::array({
				value.scale.x,value.scale.y,value.scale.z,
				value.quaternion.x,value.quaternion.y,value.quaternion.z,
				value.translate.x,value.translate.y,value.translate.z
				});
		} else if (std::holds_alternative<Vector4>(item)) {
			//float型のjson配列を登録
			Vector4 value = std::get<Vector4>(item);
			root[groupName][itemName] = json::array({ value.x,value.y,value.z,value.w });
		}
	}

	//ファイルの書き出し
	//ディレクトリが無ければ作成
	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists(kDirectoryPath)) {
		std::filesystem::create_directories(dir);
	}

	//書き込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + groupName + ".json";
	//書き込み用ファイルストリーム
	std::ofstream ofs;
	//ファイルを書き込み用に開く
	ofs.open(filePath);

	//ファイルオープン失敗をチェック
	if (ofs.fail()) {
		std::string message = "Failed open data file for write";
#ifdef USE_IMGUI
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
#endif // USE_IMGUI
		assert(0);
		return;
	}

	//ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;

	//ファイルを閉じる
	ofs.close();
}

//設定ファイルの読み込み
void GlobalVariables::LoadFile(const std::string& groupName) {
	//読み込みフラグをオン
	isLoaded_ = true;

	//読み込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + groupName + ".json";

	//読み込み用ファイルストリーム
	std::ifstream ifs;

	//ファイルを読み込み用に開く
	ifs.open(filePath);

	//ファイルオープン失敗をチェック
	if (ifs.fail()) {
		std::string message = "Failed open data file for write";
#ifdef USE_IMGUI
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
#endif // USE_IMGUI
		assert(0);
		return;
	}

	json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;

	//ファイルを閉じる
	ifs.close();

	//グループを検索
	json::iterator itGroup = root.find(groupName);

	//未登録チェック
	assert(itGroup != root.end());

	//各項目について
	for (json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); itItem++) {
		//項目名を取得
		const std::string& itemName = itItem.key();

		//項目の型によって処理を変える
		if (itItem->is_number_integer()) {
			//int32_tの値を保持していれば
			int32_t value = itItem->get<int32_t>();
			SetValue(groupName, itemName, value);
		} else if (itItem->is_number_float()) {
			//float型の値を保持していれば
			double value = itItem->get<double>();
			SetValue(groupName, itemName, static_cast<float>(value));
		} else if (itItem->is_array() && itItem->size() == 3) {
			//要素数3の配列であれば
			Vector3 value = { itItem->at(0),itItem->at(1),itItem->at(2) };
			SetValue(groupName, itemName, value);
		} else if (itItem->is_array() && itItem->size() == 9) {
			//要素数9の配列であれば
			TransformData value = { 
				{ itItem->at(0),itItem->at(1),itItem->at(2) },
				{ itItem->at(3),itItem->at(4),itItem->at(5) },
				{ itItem->at(6),itItem->at(7),itItem->at(8) }	
			};
			SetValue(groupName, itemName, value);
		} else if (itItem->is_array() && itItem->size() == 4) {
			//要素数4の配列であれば
			Vector4 value = { itItem->at(0),itItem->at(1),itItem->at(2),itItem->at(3)};
			SetValue(groupName, itemName, value);
		}
	}
}
