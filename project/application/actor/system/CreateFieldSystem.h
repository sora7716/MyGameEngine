#pragma once
#include <string>
#include <vector>
#include <memory>

//前方宣言
class BaseScene;
class MapChipField;
class GameObject;

/// <summary>
/// フィールドのブロックデータ
/// </summary>
struct FieldBlockData {
	std::vector<std::vector<std::vector<GameObject*>>> data;
};

/// <summary>
/// フィールド生成のシステム
/// </summary>
class CreateFieldSystem {
public://静的メンバ関数
	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="scene">シーン</param>
	/// <param name="mapChipCsvName">マップチップのCSVファイル名</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<CreateFieldSystem>Create(BaseScene* scene,const std::string& mapChipCsvName);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	CreateFieldSystem();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~CreateFieldSystem();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="scene">シーン</param>
	/// <param name="mapChipCsvName">マップチップのCSVファイル名</param>
	void Initialize(BaseScene* scene,const std::string& mapChipCsvName);

	/// <summary>
	/// マップチップフィールドの取得
	/// </summary>
	/// <returns>マップチップフィールド</returns>
	MapChipField* GetMapChipField();

	/// <summary>
	/// フィールドブロックデータの取得
	/// </summary>
	/// <returns>フィールドブロックデータ</returns>
	const FieldBlockData& GetFieldBlockData()const;
private://メンバ関数
	/// <summary>
	/// フィールドの生成
	/// </summary>
	void CreateField();
private://メンバ変数
	//マップチップのCSVの名前
	std::string mapChipCsvName_ = "";
	//シーン
	BaseScene* scene_ = nullptr;
	//マップチップフィールド
	std::unique_ptr<MapChipField> mapChipField_ = nullptr;
	//フィールドに配置しているブロック
	FieldBlockData fieldBlockData_;
};

