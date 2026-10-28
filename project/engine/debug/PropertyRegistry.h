#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Color.h"
#include <string>
#include <vector>
#include <variant>
#include <cstdint>
#include <functional>

//プロパティ
namespace inspector{
	//グループの開始
	struct GroupBegin{
		std::string displayName = "";
	};
	//グループの終了
	struct GroupEnd{

	};

	//浮動小数
	struct FloatProperty{
		std::string displayName = "";
		std::function<float()>getter;
		std::function<void(float)>setter;
		float dragSpeed = 0.0f;
	};
	//フラグ
	struct BoolProperty{
		std::string displayName = "";
		std::function<bool()>getter;
		std::function<void(bool)>setter;
	};
	//整数
	struct IntProperty{
		std::string displayName = "";
		std::function<int32_t()>getter;
		std::function<void(int32_t)>setter;
		float dragSpeed = 0.0f;
	};
	//Vector2
	struct Vector2Property{
		std::string displayName = "";
		std::function<Vector2()>getter;
		std::function<void(const Vector2&)>setter;
		float dragSpeed = 0.0f;
	};
	//Vector3
	struct Vector3Property{
		std::string displayName = "";
		std::function<Vector3()>getter;
		std::function<void(const Vector3&)>setter;
		float dragSpeed = 0.0f;
	};
	//Vector4
	struct Vector4Property{
		std::string displayName = "";
		std::function<Vector4()>getter;
		std::function<void(const Vector4&)>setter;
		float dragSpeed = 0.0f;
	};
	//Color
	struct ColorProperty{
		std::string displayName = "";
		std::function<Color()>getter;
		std::function<void(const Color&)>setter;
	};
	//文字列
	struct StringProperty{
		std::string displayName = "";
		std::function <std::string()>getter;
		std::function<void(const std::string&)>setter;
	};
	//Enum
	struct EnumProperty{
		std::string displayName = "";
		std::vector<std::string>options;
		std::function <uint32_t()>getter;
		std::function<void(uint32_t)>setter;
	};
}

using Property = std::variant<
	inspector::GroupBegin,
	inspector::GroupEnd,
	inspector::FloatProperty,
	inspector::BoolProperty,
	inspector::IntProperty,
	inspector::Vector2Property,
	inspector::Vector3Property,
	inspector::Vector4Property,
	inspector::ColorProperty,
	inspector::StringProperty,
	inspector::EnumProperty
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
	/// グループの開始
	/// </summary>
	/// <param name="displayName">表示名</param>
	void BeginGroup(const std::string& displayName);

	/// <summary>
	/// グループの終了
	/// </summary>
	void EndGroup();

	/// <summary>
	/// 浮動小数の追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="value">浮動小数</param>
	/// <param name="dragSpeed">ドラッグさせるスピード</param>
	void Add(const std::string& displayName, float& value, float dragSpeed = 0.1f);

	/// <summary>
	/// 浮動小数の追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="getter">Getter</param>
	/// <param name="setter">Setter</param>
	void Add(const std::string& displayName, std::function<float()> getter, std::function<void(float)>setter, float dragSpeed = 0.1f);

	/// <summary>
	/// フラグの追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="value">フラグ</param>
	void Add(const std::string& displayName, bool& value);

	/// <summary>
	/// フラグの追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="getter">Getter</param>
	/// <param name="setter">Setter</param>
	void Add(const std::string& displayName, std::function<bool()> getter, std::function<void(bool)>setter);

	/// <summary>
	/// 整数の追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="value">整数</param>
	/// <param name="dragSpeed">ドラッグさせるスピード</param>
	void Add(const std::string& displayName, int32_t& value, float dragSpeed = 0.1f);

	/// <summary>
	/// 整数の追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="getter">Getter</param>
	/// <param name="setter">Setter</param>
	void Add(const std::string& displayName, std::function<int32_t()> getter, std::function<void(int32_t)>setter);

	/// <summary>
	/// Vector2の追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="value">Vector2</param>
	/// <param name="dragSpeed">ドラッグさせるスピード</param>
	void Add(const std::string& displayName, Vector2& value, float dragSpeed = 0.1f);

	/// <summary>
	/// Vector2の追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="getter">Getter</param>
	/// <param name="setter">Setter</param>
	void Add(const std::string& displayName, std::function<Vector2()> getter, std::function<void(const Vector2&)>setter, float dragSpeed = 0.1f);

	/// <summary>
	/// Vector3の追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="value">Vector3</param>
	/// <param name="dragSpeed">ドラッグさせるスピード</param>
	void Add(const std::string& displayName, Vector3& value, float dragSpeed = 0.1f);

	/// <summary>
	/// Vector3の追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="getter">Getter</param>
	/// <param name="setter">Setter</param>
	void Add(const std::string& displayName, std::function<Vector3()> getter, std::function<void(const Vector3&)>setter);

	/// <summary>
	/// Vector4の追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="value">Vector4</param>
	/// <param name="dragSpeed">ドラッグさせるスピード</param>
	void Add(const std::string& displayName, Vector4& value, float dragSpeed = 0.1f);

	/// <summary>
	/// Vector4の追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="getter">Getter</param>
	/// <param name="setter">Setter</param>
	void Add(const std::string& displayName, std::function<Vector4()> getter, std::function<void(const Vector4&)>setter);

	/// <summary>
	/// Colorの追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="value">Color</param>
	void Add(const std::string& displayName, Color& value);

	/// <summary>
	/// Colorの追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="getter">Getter</param>
	/// <param name="setter">Setter</param>
	void Add(const std::string& displayName, std::function<Color()> getter, std::function<void(const Color&)>setter);

	/// <summary>
	/// 文字列の追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="value">文字列</param>
	void Add(const std::string& displayName, std::string& value);

	/// <summary>
	/// 文字列の追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="getter">Getter</param>
	/// <param name="setter">Setter</param>
	void Add(const std::string& displayName, std::function<std::string()> getter, std::function<void(const std::string&)>setter);

	/// <summary>
	/// 列挙型の追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="options">オプション</param>
	/// <param name="getter">Getter</param>
	/// <param name="setter">Setter</param>
	void Add(const std::string& displayName, const std::vector<std::string>& options, std::function<uint32_t()> getter, std::function<void(uint32_t)>setter);

	/// <summary>
	/// 列挙型の追加
	/// </summary>
	/// <param name="displayName">表示名</param>
	/// <param name="options">オプション</param>
	/// <param name="getter">Getter</param>
	/// <param name="setter">Setter</param>
	void Add(const std::string& displayName, const std::vector<std::string>& options, uint32_t& value);

	/// <summary>
	/// 追加
	/// </summary>
	/// <typeparam name="T">操作対象の型</typeparam>
	/// <typeparam name="Value">Inspectorで扱う型</typeparam>
	/// <typeparam name="Arg">Setterが受け取る型</typeparam>
	/// <param name="displayName">表示名</param>
	/// <param name="value">値</param>
	/// <param name="object">オブジェクト</param>
	/// <param name="index">検索キー</param>
	/// <param name="setter">セッター</param>
	template <typename T, typename Value, typename Setter>
	void Add(
		const std::string& displayName,
		const Value& value,
		T* object,
		uint32_t index,
		Setter setter){
		const Value* valuePtr = &value;
		std::function<Value()>getter = [valuePtr]()->Value{return *valuePtr; };

		std::function<void(const Value&)>setterFunc = [object, index, setter](const Value& value){(object->*setter)(index, value); };
		Add(displayName, getter, setterFunc);
	}

	/// <summary>
	/// 追加
	/// </summary>
	/// <typeparam name="T">操作対象の型</typeparam>
	/// <typeparam name="Value">Inspectorで扱う型</typeparam>
	/// <typeparam name="Arg">Setterが受け取る型</typeparam>
	/// <param name="displayName">表示名</param>
	/// <param name="value">値</param>
	/// <param name="object">オブジェクト</param>
	/// <param name="index">検索キー</param>
	/// <param name="setter">セッターの関数ポインタ</param>
	/// <param name="dragSpeed">ドラッグする速度</param>
	template <typename T, typename Value, typename Setter>
	void Add(
		const std::string& displayName,
		const Value& value,
		T* object,
		uint32_t index,
		Setter setter,
		float dragSpeed){
		const Value* valuePtr = &value;
		std::function<Value()>getter = [valuePtr]()->Value{return *valuePtr; };

		std::function<void(const Value&)>setterFunc = [object, index, setter](const Value& value){(object->*setter)(index, value); };
		Add(displayName, getter, setterFunc, dragSpeed);
	}

	/// <summary>
	/// プロパティの配列の取得
	/// </summary>
	/// <returns>プロパティの配列</returns>
	const std::vector<Property>& GetProperties()const;
private://メンバ変数
	//プロパティの配列
	std::vector<Property>properties_;
};