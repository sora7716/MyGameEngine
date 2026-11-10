#include "MapChipField.h"
#include <map>
#include <fstream>
#include <sstream>
#include <cassert>
//マップチップの読み込み
void MapChipField::LoadMapChipCsv(const std::string& filePath) {
	//マップチップデータのリセット
	ResetMapChipData();
	//ファイルを開く
	std::ifstream file;//1行ずつ読み込む(すべてを一気に読み込むわけではない)
	file.open(directoryPath_ + filePath);
	assert(file.is_open());

	//マップチップCSV
	std::stringstream mapChipCsv;//1文字ずつ読み込んでいく

	//ファイルの内容を文字列ストリームにコピー
	mapChipCsv << file.rdbuf();//文字のコピーをfileに入れていく

	//ファイルを閉じる
	file.close();

	//CSVからマップチップデータを読み込む
	std::string line;
	while(std::getline(mapChipCsv,line)) {
		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);//文字列のstream
		//行を追加
		std::vector<MapChipStatus> lines;
		std::string word;
		//','(区切り文字),word(1文字)
		while(std::getline(line_stream,word,',')) {
			MapChipStatus status = {};
			//空気だった場合
			if(word == "0") {
				status.type = MapChipType::kBlank;
				status.count = 1;
				lines.push_back(status);
				continue;
			} else if(word == "B") {
				//ブロックだった場合
				status.type = MapChipType::kBlock;
				status.count = 1;
				lines.push_back(status);
				continue;
			}
			//ブロックだった場合
			status.type = MapChipType::kBlock;
			int32_t count = static_cast<uint32_t>(std::stoi(word.substr(1)));
			if(count < 0) {
				status.count = 1;
			} else {
				status.count = count;
			}
			lines.push_back(status);
		}
		//マップの横幅を設定
		horizontal_ = static_cast<uint32_t>(lines.size());
		//マップチップのデータに追加
		mapChipData_.data.push_back(lines);
	}

	//奥行も設定
	longitudinal_ = static_cast<uint32_t>(mapChipData_.data.size());
}

//マップチップデータの取得
const MapChipData& MapChipField::GetMapChipData() const {
	return mapChipData_;
}

//マップのインデックスを取得
MapChipField::IndexSet MapChipField::GetMapChipIndex(const Vector3& pos) {
	IndexSet result = {
		.x = static_cast<uint32_t>(pos.x - origin_.x) / kBlockWidth,
		.z = static_cast<uint32_t>(pos.z - origin_.z) / kBlockDepth,
	};
	return result;
}

//マップからいる位置を取得
Vector3 MapChipField::GetMapChipPosition(const IndexSet& indexSet) {
	Vector3 result = {
		.x = origin_.x + (static_cast<float>(indexSet.x) + 0.5f) * kBlockWidth,
		.z = origin_.z + (static_cast<float>(indexSet.z) + 0.5f) * kBlockDepth,
	};
	return result;
}

//いる位置のマップのステータスを取得
MapChipStatus MapChipField::GetMapChipStatus(const Vector3& pos) {
	//現在いる位置のマップ番号を取得
	IndexSet index = GetMapChipIndex(pos);
	//マップステータス
	MapChipStatus status = {.type = MapChipType::kBlank,.count = 1};
	//もし範囲外なら
	if(horizontal_ <= index.x || longitudinal_ <= index.z) {
		return status;
	}

	return mapChipData_.data[index.z][index.x];
}

//いる位置のマップのステータスを取得
MapChipStatus MapChipField::GetMapChipStatus(const IndexSet& indexSet) {
	//マップステータス
	MapChipStatus status = {.type = MapChipType::kBlank,.count = 1};
	//もし範囲外なら
	if(horizontal_ <= indexSet.x || longitudinal_ <= indexSet.z) {
		return status;
	}
	return mapChipData_.data[indexSet.z][indexSet.x];
}

// マップチップデータのリセット
void MapChipField::ResetMapChipData() {
	mapChipData_.data.clear();
}