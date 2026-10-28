#include "PropertyRegistry.h"
using namespace inspector;

//コンストラクタ
PropertyRegistry::PropertyRegistry(){
}

//デストラクタ
PropertyRegistry::~PropertyRegistry(){
}

//グループの開始
void PropertyRegistry::BeginGroup(const std::string& displayName){
	GroupBegin groupBegin = { displayName };
	properties_.push_back(groupBegin);
}

//グループの終了
void PropertyRegistry::EndGroup(){
	GroupEnd groupEnd = {};
	properties_.push_back(groupEnd);
}

//浮動小数の追加
void PropertyRegistry::Add(const std::string& displayName, float& value, float dragSpeed){
	FloatProperty property = {
		displayName,
		[&value](){
			return value;
		},
		[&value](float newValue){
			value = newValue;
		},
		dragSpeed
	};
	properties_.push_back(property);
}

//浮動小数の追加
void PropertyRegistry::Add(const std::string& displayName, std::function<float()> getter, std::function<void(float)>setter, float dragSpeed){
	FloatProperty property = { displayName,getter,setter,dragSpeed };
	properties_.push_back(property);
}

//フラグの追加
void PropertyRegistry::Add(const std::string& displayName, bool& value){
	BoolProperty property = {
		displayName,
		[&value](){
			return value;
		},
		[&value](bool newValue){
			value = newValue;
		}
	};
	properties_.push_back(property);
}

//フラグの追加
void PropertyRegistry::Add(const std::string& displayName, std::function<bool()> getter, std::function<void(bool)>setter){
	BoolProperty property = { displayName,getter,setter };
	properties_.push_back(property);
}

//整数の追加
void PropertyRegistry::Add(const std::string& displayName, int32_t& value, float dragSpeed){
	IntProperty property = {
		displayName,
		[&value](){
			return value;
		},
		[&value](int32_t newValue){
			value = newValue;
		},
		dragSpeed
	};
	properties_.push_back(property);
}

//整数の追加
void PropertyRegistry::Add(const std::string& displayName, std::function<int32_t()> getter, std::function<void(int32_t)>setter){
	IntProperty property = { displayName,getter,setter };
	properties_.push_back(property);
}

//Vector2の追加
void PropertyRegistry::Add(const std::string& displayName, Vector2& value, float dragSpeed){
	Vector2Property property = {
		displayName,
		[&value](){
			return value;
		},
		[&value](const Vector2& newValue){
			value = newValue;
		},
		dragSpeed
	};
	properties_.push_back(property);

}

//Vector2の追加
void PropertyRegistry::Add(const std::string& displayName, std::function<Vector2()> getter, std::function<void(const Vector2&)>setter, float dragSpeed){
	Vector2Property property = { displayName,getter,setter,dragSpeed };
	properties_.push_back(property);
}

//Vector3の追加
void PropertyRegistry::Add(const std::string& displayName, Vector3& value, float dragSpeed){
	Vector3Property property = {
		displayName,
		[&value](){
			return value;
		},
		[&value](const Vector3& newValue){
			value = newValue;
		},
		dragSpeed
	};
	properties_.push_back(property);
}

//Vector3の追加
void PropertyRegistry::Add(const std::string& displayName, std::function<Vector3()> getter, std::function<void(const Vector3&)>setter){
	Vector3Property property = { displayName,getter,setter };
	properties_.push_back(property);
}

//Vector4の追加
void PropertyRegistry::Add(const std::string& displayName, Vector4& value, float dragSpeed){
	Vector4Property property = {
		displayName,
		[&value](){
			return value;
		},
		[&value](const Vector4& newValue){
			value = newValue;
		},
		dragSpeed
	};
	properties_.push_back(property);
}

//Vector4の追加
void PropertyRegistry::Add(const std::string& displayName, std::function<Vector4()> getter, std::function<void(const Vector4&)>setter){
	Vector4Property property = { displayName,getter,setter };
	properties_.push_back(property);
}

//Colorの追加
void PropertyRegistry::Add(const std::string& displayName, Color& value){
	ColorProperty property = { displayName,
		[&value](){
			return value;
		},
		[&value](const Color& newValue){
			value = newValue;
		}
	};
	properties_.push_back(property);
}

//Colorの追加
void PropertyRegistry::Add(const std::string& displayName, std::function<Color()> getter, std::function<void(const Color&)>setter){
	ColorProperty property = { displayName,getter,setter };
	properties_.push_back(property);
}

//Stringの追加
void PropertyRegistry::Add(const std::string& displayName, std::string& value){
	StringProperty property = {
		displayName,
		[&value](){
			return value;
		},
		[&value](const std::string& newValue){
			value = newValue;
		}
	};
	properties_.push_back(property);
}

//Stringの追加
void PropertyRegistry::Add(const std::string& displayName, std::function<std::string()> getter, std::function<void(const  std::string&)>setter){
	StringProperty property = { displayName,getter,setter };
	properties_.push_back(property);
}

//列挙型の追加
void PropertyRegistry::Add(const std::string& displayName, const std::vector<std::string>& options, std::function<uint32_t()> getter, std::function<void(uint32_t)> setter){
	EnumProperty enumProperty = { displayName,options,getter,setter };
	properties_.push_back(enumProperty);
}

//列挙型の追加
void PropertyRegistry::Add(const std::string& displayName, const std::vector<std::string>& options, uint32_t& value){
	EnumProperty property = { displayName,
		options,
		[&value](){
			return value;
		},
		[&value](uint32_t newValue){
			value = newValue;
		}
	};
	properties_.push_back(property);
}

//プロパティの配列の取得
const std::vector<Property>& PropertyRegistry::GetProperties() const{
	return properties_;
}
