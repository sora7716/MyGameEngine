#pragma once
#include "RenderData.h"

//スロット
struct MaterialInstanceSlot{
	Material material = {};
	MaterialTexturePaths texturePaths = {};
	Transform2d uvTransform = {};
};

/// <summary>
/// マテリアルのインスタンス
/// </summary>
class MaterialInstance{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	MaterialInstance();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~MaterialInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="texturePaths"></param>
	void Initialize(const std::vector<MaterialTexturePaths>& texturePaths);

	/// <summary>
	/// 色の設定
	/// </summary>
	/// <param name="index">検索キー</param>
	/// <param name="color">色</param>
	void SetColor(uint32_t index, const Vector4& color);

	/// <summary>
	/// テクスチャの設定
	/// </summary>
	/// <param name="index">検索キー</param>
	/// <param name="path">テクスチャパス</param>
	void SetTexture(uint32_t index, const std::string& path);

	/// <summary>
	/// 環境マップの設定
	/// </summary>
	/// <param name="index">検索キー</param>
	/// <param name="path">環境マップパス</param>
	void SetEnvironmentMap(uint32_t index, const std::string& path);

	/// <summary>
	/// UVトランスフォームの設定
	/// </summary>
	/// <param name="index">検索キー</param>
	/// <param name="transform">UVトランスフォーム</param>
	void SetUVTransform(uint32_t index, const Transform2d& transform);

	/// <summary>
	/// ライティングするかの設定
	/// </summary>
	/// <param name="index">検索キー</param>
	/// <param name="enabled">ライティングするかのフラグ</param>
	void SetIsLighting(uint32_t index, bool enabled);

	/// <summary>
	/// 光沢度の設定
	/// </summary>
	/// <param name="index">検索キー</param>
	/// <param name="shininess">光沢度</param>
	void SetShininess(uint32_t index, float shininess);

	/// <summary>
	/// 環境マップの映り込み度の設定
	/// </summary>
	/// <param name="index">検索キー</param>
	/// <param name="coefficient">環境マップの映り込み度</param>
	void SetEnvironmentCoefficient(uint32_t index, float coefficient);

	/// <summary>
	/// リムライトの色の設定
	/// </summary>
	/// <param name="index">検索キー</param>
	/// <param name="color">リムライトの色</param>
	void SetRimColor(const Vector4& color);

	/// <summary>
	/// リムライトの強さの設定
	/// </summary>
	/// <param name="index">検索キー</param>
	/// <param name="power">リムライトの強さ</param>
	void SetRimPower(float power);

	/// <summary>
	/// リムライトのアウトラインの強さの設定
	/// </summary>
	/// <param name="index">検索キー</param>
	/// <param name="outLinePower">リムライトのアウトラインの強さ</param>
	void SetRimOutLinePower(float outLinePower);

	/// <summary>
	/// リムライトの柔らかさの設定
	/// </summary>
	/// <param name="index">検索キー</param>
	/// <param name="softness">リムライトの柔らかさ</param>
	void SetRimSoftness(float softness);

	/// <summary>
	/// リムライトをするかの設定
	/// </summary>
	/// <param name="index">検索キー</param>
	/// <param name="enabled">リムライトをするかのフラグ</param>
	void SetIsRimLighting(bool enabled);

	/// <summary>
	/// リムライトの設定
	/// </summary>
	/// <param name="rimLight">リムライト</param>
	void SetRimLight(const RimLight& rimLight);

	/// <summary>
	/// スロットの取得
	/// </summary>
	/// <returns>スロット</returns>
	const std::vector<MaterialInstanceSlot>& GetSlots()const;

	/// <summary>
	/// リムライトを取得
	/// </summary>
	/// <returns>リムライト</returns>
	const RimLight& GetRimLight()const;

	/// <summary>
	/// マテリアルが変更されたかを判断する変数の取得
	/// </summary>
	/// <returns>マテリアルが変更されたかを判断する変数</returns>
	uint64_t GetRevision()const;
private://メンバ変数
	//マテリアルインスタンスのスロット
	std::vector<MaterialInstanceSlot>slots_;
	//リムライト
	RimLight rimLight_ = {};
	//マテリアルが変更されたかを判断する変数
	uint64_t revision_ = 0;
};

