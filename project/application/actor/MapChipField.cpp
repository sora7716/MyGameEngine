#include "MapChipField.h"
#include <fstream>
#include <sstream>
#include <cassert>

//マップチップのデータのリセット
void MapChipField::ResetMapChipDate() {
	mapChipData_.clear();
	//横幅のサイズを設定
	mapChipData_.resize(kMapWidth);
	for (std::vector<std::vector<int32_t>>& height : mapChipData_) {
		//縦幅のサイズを設定
		height.resize(kMapHeight);
		for (std::vector <int32_t>& depth : height) {
			//奥行のサイズを設定
			depth.resize(kMapDepth);
		}
	}
}

//CSVの読み込み
void MapChipField::LoadCSV(const std::string& filePath) {
	//マップチップデータをリセット
	ResetMapChipDate();

	//ファイルを開く
	std::ifstream file;//一行ずつ読み込む(すべて一気に読み込むわけではない)
	file.open(filePath);
	assert(file.is_open());

	//マップチップＣＳＶ
	std::stringstream mapChipCsv;//一文字ずつ読み込んでいく

}
