#pragma once
#include <cstdint>
#include <vector>
#include <string>

/// <summary>
/// マップチップフィールド
/// </summary>
class MapChipField{
public://メンバ関数
	/// <summary>
	/// マップチップのデータをリセット
	/// </summary>
	void ResetMapChipDate();

	/// <summary>
    /// CSVの読み込み
    /// </summary>
	/// <param name="filePath">ファイルパス</param>
	void LoadCSV(const std::string& filePath);
private://定数
	//マップの横幅
	static inline const int32_t kMapWidth = 20;
	//マップの縦幅
	static inline const int32_t kMapHeight = 20;
	//マップの奥行
	static inline const int32_t kMapDepth = 20;
private://メンバ変数
	//マップ
	std::vector<std::vector<std::vector<int32_t>>>mapChipData_;
};

