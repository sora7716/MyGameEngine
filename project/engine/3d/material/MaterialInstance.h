#pragma once
#include "RenderData.h"
#include "ShaderInfo.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Color.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <cstddef>
#include <concepts>

//前方宣言
class ShaderManager;

//スロット
struct MaterialInstanceSlot {
	Material material = {};
	MaterialTexturePaths texturePaths = {};
	RectTransform uvTransform = {};
};

//拡散反射の反射モデル
enum DiffuseModel :uint32_t {
	kLambert ,
	kHalfLambert
};

//鏡面反射の反射モデル
enum SpecularModel :uint32_t {
	kPhong ,
	kBlingPhong
};

/// <summary>
/// マテリアルのインスタンス
/// </summary>
class MaterialInstance {
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
	void Initialize(const MaterialTexturePaths& texturePaths , ShaderManager* shaderManager);

	/// <summary>
	/// テクスチャの設定
	/// </summary>
	/// <param name="path">テクスチャパス</param>
	void SetTexture(const std::string& path);

	/// <summary>
	/// 環境マップの設定
	/// </summary>
	/// <param name="path">環境マップパス</param>
	void SetEnvironmentMap(const std::string& path);

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
	/// リムライトを取得
	/// </summary>
	/// <returns>リムライト</returns>
	const RimLight& GetRimLight()const;

	/// <summary>
	/// ピクセルシェーダのファイルパスを取得
	/// </summary>
	/// <param name="pixelShaderFileName">ピクセルシェーダファイル名</param>
	/// <param name="cBufferName">コンスタントバッファ名</param>
	void SetPixelShaderFileName(const std::string& pixelShaderFileName , const std::string& cBufferName);

	/// <summary>
	/// パラメータの設定
	/// </summary>
	/// <typeparam name="T">型</typeparam>
	/// <param name="name">名前</param>
	/// <param name="value">値</param>
	template<typename T>
	void SetParameter(const std::string& name , T value) {
		//存在しなかったら
		if (!parameterInfos_.contains(name)) {
			return;
		}
		ParameterInfo parameterInfo = parameterInfos_.at(name);

		//型とサイズが一致していなかった場合
		if (parameterInfo.size != sizeof(value)) {
			return;
		}

		//サイズの範囲内か確認
		if (parameterInfo.offset > parameterValues_.size() || parameterInfo.size > parameterValues_.size() - parameterInfo.offset) {
			return;
		}

		//書き込む先を取得
		std::byte* parameterValue = parameterValues_.data() + parameterInfo.offset;
		switch (parameterInfo.type) {
			case ParameterType::kFloat: {
					if (!std::same_as<T , float>) {
						return;
					}
					break;
				}
			case ParameterType::kVector2: {
					if (!std::same_as<T , Vector2>) {
						return;
					}
					break;
				}
			case ParameterType::kVector3: {
					if (!std::same_as<T , Vector3>) {
						return;
					}
					break;
				}
			case ParameterType::kColor: {
					if (!std::same_as<T , Color>) {
						return;
					}
					break;
				}
			case ParameterType::kInt: {
					if (!std::same_as<T , int32_t>) {
						return;
					}
					break;
				}
			case ParameterType::kUint: {
					if (!std::same_as<T , uint32_t>) {
						return;
					}
					break;
				}
			default: {
					return;
					break;
				}
		}
		std::memcpy(parameterValue , &value , parameterInfo.size);

		//変更があったので加算
		revision_++;
	}

	/// <summary>
	/// マテリアルが変更されたかを判断する変数の取得
	/// </summary>
	/// <returns>マテリアルが変更されたかを判断する変数</returns>
	uint64_t GetRevision()const;

	/// <summary>
	/// コンスタントバッファの情報を取得
	/// </summary>
	/// <returns>コンスタントバッファの情報</returns>
	const ConstantBufferInfo& GetCBufferInfo()const;

	/// <summary>
	/// パラメータのメモリの値を取得
	/// </summary>
	/// <returns>パラメータの値</returns>
	const std::vector<std::byte>& GetParameterValues()const;

	/// <summary>
	/// テクスチャファイルパスの取得
	/// </summary>
	/// <returns>テクスチャファイルパス</returns>
	const MaterialTexturePaths& GetTextureFilePaths()const;
private://メンバ変数
	//シェーダの管理
	ShaderManager* shaderManager_ = nullptr;
	//ピクセルシェーダの名
	std::string pixelShaderFileName_ = "";
	//テクスチャのファイルパス
	MaterialTexturePaths texturePaths_ = {};
	//リムライト
	RimLight rimLight_ = {};
	//マテリアルが変更されたかを判断する変数
	uint64_t revision_ = 0;

	//コンスタントバッファの情報を取得
	ConstantBufferInfo cBuffer_ = {};
	//パラメータの情報
	std::unordered_map<std::string , ParameterInfo>parameterInfos_;
	//パラメータのバイト
	std::vector<std::byte>parameterValues_;
};

