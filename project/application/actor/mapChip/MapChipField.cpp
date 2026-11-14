#include "MapChipField.h"
#include <map>
#include <fstream>
#include <sstream>
#include <cassert>
//生成
std::unique_ptr<MapChipField> MapChipField::Create(const std::string& fileName) {
	std::unique_ptr<MapChipField>instance = std::make_unique<MapChipField>();
	instance->Initialize(fileName);
	return instance;
}

//コンストラクタ
MapChipField::MapChipField() {
}

//デストラクタ
MapChipField::~MapChipField() {
}

//初期化
void MapChipField::Initialize(const std::string& fileName) {
	//ファイルの読み込み
	LoadMapChipCsv(fileName);

	//マップ全体の半径を求める
	mapRadius = {
		static_cast<float>(horizontal_) * kBlockWidth / 2.0f,
		kBlockHeight,
		static_cast<float>(longitudinal_) * kBlockDepth / 2.0f,
	};

	//オリジンを設定
	origin_.x = -mapRadius.x - kBlockWidth / 2.0f;
	origin_.y = -kBlockHeight / 2.0f;
	origin_.z = -mapRadius.z - kBlockDepth / 2.0f;
}

//マップチップデータの取得
const MapChipData& MapChipField::GetMapChipData() const {
	return mapChipData_;
}

//マップのインデックスを取得
Vector3Int MapChipField::GetMapChipIndexByPosition(const Vector3& pos) {
	//切り捨てを行った値
	Vector3 roundDown = (pos - origin_).Floor();
	Vector3Int result = {
		.x = static_cast<int32_t>(roundDown.x / kBlockWidth),
		.y = static_cast<int32_t>(roundDown.y / kBlockHeight),
		.z = static_cast<int32_t>(roundDown.z / kBlockDepth),
	};

	return result;
}

//マップから位置を取得
Vector3 MapChipField::GetMapChipPositionByIndex(const Vector3Int& index) {
	Vector3 result = {
		.x = origin_.x + (static_cast<float>(index.x) + 0.5f) * kBlockWidth,
		.y = origin_.y + index.y * kBlockHeight,
		.z = origin_.z + (static_cast<float>(index.z) + 0.5f) * kBlockDepth,
	};
	return result;
}

//位置のマップのステータスを取得
MapChipStatus MapChipField::GetMapChipStatus(const Vector3& pos) {
	//現在いる位置のマップ番号を取得
	Vector3Int index = GetMapChipIndexByPosition(pos);
	//マップステータス
	MapChipStatus status = {.type = MapChipType::kBlank,.count = 1};
	//範囲外なら何もしない
	if(index.x < 0 || index.y < 0 || index.z < 0 || index.x >= horizontal_ || index.z >= longitudinal_) {
		return status;
	}

	//現在のステータスを取得
	status = mapChipData_.data[index.z][index.x];
	//位置がステータスより上を指していたら
	if(status.count <= static_cast<uint32_t>(index.y)) {
		status = {.type = MapChipType::kBlank,.count = 1};
		return status;
	}

	return status;
}

//マップのステータス
MapChipType MapChipField::GetMapChipTypeByIndex(const Vector3Int& index) {
	//範囲外なら何もしない
	if(index.x < 0 || index.y < 0 || index.z < 0 || index.x >= horizontal_ || index.z >= longitudinal_) {
		return MapChipType::kBlank;
	}
	//位置を取得
	Vector3 pos = GetMapChipPositionByIndex(index);
	//ステータスの取得
	MapChipStatus status = GetMapChipStatus(pos);
	//積んである高さより上は何もない
	if(status.count <= static_cast<uint32_t>(index.y)) {
		return MapChipType::kBlank;
	}

	return status.type;
}

// 範囲矩形を取得
MapChipField::Rect MapChipField::GetRectByIndex(Vector3Int index) {
	// 指定したブロックを中心座標を取得する
	Vector3 center = GetMapChipPositionByIndex(index);

	Rect rect;
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.bottom = center.y - kBlockHeight / 2.0f;
	rect.top = center.y + kBlockHeight / 2.0f;
	rect.front = center.z + kBlockDepth / 2.0f;
	rect.back = center.z - kBlockDepth / 2.0f;

	return rect;
}

// マップチップデータのリセット
void MapChipField::ResetMapChipData() {
	mapChipData_.data.clear();
}

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