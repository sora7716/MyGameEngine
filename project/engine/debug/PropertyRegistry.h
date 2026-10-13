#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Color.h"
#include <string>
#include <vector>
#include <variant>
#include <cstdint>

//プロパティ
namespace inspector{
	//浮動小数
	struct FloatProperty{
		std::string displayName = "";
		float* value = nullptr;
		float dragSpeed = 0.0f;
	};
	//フラグ
	struct BoolProperty{
		std::string displayName = "";
		bool* value = nullptr;
	};
	//整数
	struct IntProperty{
		std::string displayName = "";
		int32_t* value = nullptr;
		float dragSpeed = 0.0f;
	};
	//Vector2
	struct Vector2Property{
		std::string displayName = "";
		Vector2* value = nullptr;
		float dragSpeed = 0.0f;
	};
	//Vector3
	struct Vector3Property{
		std::string displayName = "";
		Vector3* value = nullptr;
		float dragSpeed = 0.0f;
	};
	//Vector4
	struct Vector4Property{
		std::string displayName = "";
		Vector4* value = nullptr;
		float dragSpeed = 0.0f;
	};
	//Colorの
	struct ColorProperty{
		std::string displayName = "";
		Color* value = nullptr;
	};
	//文字列の
	struct StringProperty{
		std::string displayName = "";
		std::string* value = nullptr;
	};
}

using Property = std::variant<
	inspector::FloatProperty,
	inspector::BoolProperty,
	inspector::IntProperty,
	inspector::Vector2Property,
	inspector::Vector3Property,
	inspector::Vector4Property,
	inspector::ColorProperty,
	inspector::StringProperty
>;

//プロパティをまとめる
class PropertyRegistry{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PropertyRegistry();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PropertyRegistry();

	/// <summary>
	/// 浮動小数の追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="value">浮動小数</param>
	/// <param name="dragSpeed">ドラッグさせるスピード</param>
	void Add(const std::string& displayName, float& value, float dragSpeed = 0.1f);

	/// <summary>
	/// フラグの追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="value">フラグ</param>
	void Add(const std::string& displayName, bool& value);

	/// <summary>
	/// 整数の追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="value">整数</param>
	/// <param name="dragSpeed">ドラッグさせるスピード</param>
	void Add(const std::string& displayName, int32_t& value, float dragSpeed = 0.1f);

	/// <summary>
	/// Vector2の追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="value">Vector2</param>
	/// <param name="dragSpeed">ドラッグさせるスピード</param>
	void Add(const std::string& displayName, Vector2& value, float dragSpeed = 0.1f);

	/// <summary>
	/// Vector3の追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="value">Vector3</param>
	/// <param name="dragSpeed">ドラッグさせるスピード</param>
	void Add(const std::string& displayName, Vector3& value, float dragSpeed = 0.1f);

	/// <summary>
	/// Vector4の追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="value">Vector4</param>
	/// <param name="dragSpeed">ドラッグさせるスピード</param>
	void Add(const std::string& displayName, Vector4& value, float dragSpeed = 0.1f);

	/// <summary>
	/// Colorの追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="value">Color</param>
	void Add(const std::string& displayName, Color& value);
	
	/// <summary>
	/// 文字列の追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="value">文字列</param>
	void Add(const std::string& displayName, std::string& value);

	/// <summary>
	/// プロパティの配列の取得
	/// </summary>
	/// <returns>プロパティの配列</returns>
	const std::vector<Property>& GetProperties()const;
private://メンバ変数
	//プロパティの配列
	std::vector<Property>properties_;
};