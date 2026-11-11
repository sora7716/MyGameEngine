#pragma once
#include "MapChipData.h"
#include "Vector3.h"
#include <memory>

//マップチップフィールド
class MapChipField {
public://静的メンバ関数
	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<MapChipField> Create(const std::string& fileName);
public://構造体
	//範囲矩形
	struct Rect {
		float left;//左端
		float right;//右端
		float bottom;//下端
		float top;//上端
	};

	//インデックスセット
	struct IndexSet {
		int32_t x;
		int32_t z;
	};
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	MapChipField();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~MapChipField();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	void Initialize(const std::string& fileName);

	/// <summary>
	/// マップチップのデータを取得
	/// </summary>
	/// <returns>マップチップデータ</returns>
	const MapChipData& GetMapChipData()const;

	/// <summary>
	/// マップのインデックスを取得
	/// </summary>
	/// <param name="pos">位置</param>
	/// <returns>マップのインデックス</returns>
	IndexSet GetMapChipIndex(const Vector3& pos);

	/// <summary>
	/// マップからいる位置を取得
	/// </summary>
	/// <param name="indexSet">インデックス</param>
	/// <returns>マップにいるの位置</returns>
	Vector3 GetMapChipPosition(const IndexSet& indexSet);

	/// <summary>
	/// いる位置のマップのステータスを取得
	/// </summary>
	/// <param name="pos">位置</param>
	/// <returns>マップのステータス</returns>
	MapChipStatus GetMapChipStatus(const Vector3& pos,float size);
private://メンバ関数
	/// <summary>
	/// マップチップのリセット
	/// </summary>
	void ResetMapChipData();

	/// <summary>
	/// マップチップの読み込み
	/// </summary>
	/// <param name="filePath">ファイル名</param>
	void LoadMapChipCsv(const std::string& filePath);
public://定数
	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	static inline const float kBlockDepth = 1.0f;
private://メンバ変数
	//マップ全体の横幅
	int32_t horizontal_ = 0;
	//マップ全体の奥行
	int32_t longitudinal_ = 0;
	//マップの中心
	Vector3 origin_ = {0.0f,0.0f,0.0f};
	//半径
	Vector3 mapRadius = {};
	//ディレクトリパス
	std::string directoryPath_ = "engine/resources/csv/";
	//マップチップのデータ
	MapChipData mapChipData_;
};
