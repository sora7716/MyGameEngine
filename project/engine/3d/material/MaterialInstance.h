#pragma once
#include "RenderData.h"
#include "ShaderInfo.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <cstddef>

//前方宣言
class ShaderManager;

//スロット
struct MaterialInstanceSlot{
	Material material = {};
	MaterialTexturePaths texturePaths = {};
	RectTransform uvTransform = {};
};

//拡散反射の反射モデル
enum DiffuseModel :uint32_t{
	kLambert,
	kHalfLambert
};

//鏡面反射の反射モデル
enum SpecularModel :uint32_t{
	kPhong,
	kBlingPhong
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
	/// <param name="texturePaths">テクスチャのパス</param>
	/// <param name="shaderManager">シェーダの管理</param>
	void Initialize(const std::vector<MaterialTexturePaths>& texturePaths, ShaderManager* shaderManager);

	/// <summary>
	/// 色の設定
	/// </summary>
	/// <param name="index">検索キー</param>
	/// <param name="color">色</param>
	void SetColor(uint32_t index, const Color& color);

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
	/// UVのスケールの設定
	/// </summary>
	/// <param name="index">検索キー</param>
	/// <param name="scale">スケール</param>
	void SetUVScale(uint32_t index, const Vector2& scale);

	/// <summary>
	/// UVの回転の設定
	/// </summary>
	/// <param name="index">検索キー</param>
	/// <param name="rotate">回転</param>
	void SetUVRotate(uint32_t index, float rotate);

	/// <summary>
	/// UVの平行移動の設定
	/// </summary>
	/// <param name="index">検索キー</param>
	/// <param name="translate">平行移動</param>
	void SetUVTranslate(uint32_t index, const Vector2& translate);

	/// <summary>
	/// UVトランスフォームの設定
	/// </summary>
	/// <param name="index">検索キー</param>
	/// <param name="transform">UVトランスフォーム</param>
	void SetUVTransform(uint32_t index, const RectTransform& transform);

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
	/// 拡散反射のモデル切り替え
	/// </summary>
	/// <param name="index">検索キー</param>
	/// <param name="diffuseModel">拡散反射のモデル</param>
	void SetDiffuseModel(uint32_t index, DiffuseModel diffuseModel);

	/// <summary>
	/// 鏡面反射のモデル切り替え
	/// </summary>
	/// <param name="index">検索キー</param>
	/// <param name="specularModel">鏡面反射のモデル</param>
	void SetSpecularModel(uint32_t index, SpecularModel specularModel);

	/// <summary>
	/// リムライトの色の設定
	/// </summary>
	/// <param name="index">検索キー</param>
	/// <param name="color">リムライトの色</param>
	void SetRimColor(const Color& color);

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
	//シェーダの管理
	ShaderManager* shaderManager_ = nullptr;
	//マテリアルインスタンスのスロット
	std::vector<MaterialInstanceSlot>slots_;
	//リムライト
	RimLight rimLight_ = {};
	//マテリアルが変更されたかを判断する変数
	uint64_t revision_ = 0;

	//パラメータの情報
	std::unordered_map<std::string, ParameterInfo>parameterInfos_;
	//パラメータのバイト
	std::vector<std::byte>parameterValues_;
};

