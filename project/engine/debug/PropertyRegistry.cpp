#include "PropertyRegistry.h"
using namespace inspector;

//コンストラクタ
PropertyRegistry::PropertyRegistry(){
}

//デストラクタ
PropertyRegistry::~PropertyRegistry(){
}

//浮動小数の追加
void PropertyRegistry::Add(const std::string& displayName, float& value, float dragSpeed){
	FloatProperty property = { displayName,&value,dragSpeed };
	properties_.push_back(property);
}

//フラグの追加
void PropertyRegistry::Add(const std::string& displayName, bool& value){
	BoolProperty property = { displayName,&value };
	properties_.push_back(property);
}

//整数の追加
void PropertyRegistry::Add(const std::string& displayName, int32_t& value, float dragSpeed){
	IntProperty property = { displayName,&value,dragSpeed };
	properties_.push_back(property);
}

//Vector2の追加
void PropertyRegistry::Add(const std::string& displayName, Vector2& value, float dragSpeed){
	Vector2Property property = { displayName,&value,dragSpeed };
	properties_.push_back(property);

}

//Vector3の追加
void PropertyRegistry::Add(const std::string& displayName, Vector3& value, float dragSpeed){
	Vector3Property property = { displayName,&value,dragSpeed };
	properties_.push_back(property);
}

//Vector4の追加
void PropertyRegistry::Add(const std::string& displayName, Vector4& value, float dragSpeed){
	Vector4Property property = { displayName,&value,dragSpeed };
	properties_.push_back(property);
}

//Colorの追加
void PropertyRegistry::Add(const std::string& displayName, Color& value){
	ColorProperty property = { displayName,&value };
	properties_.push_back(property);
}

//Stringの追加
void PropertyRegistry::Add(const std::string& displayName, std::string& value){
	StringProperty property = { displayName,&value };
	properties_.push_back(property);
}

//プロパティの配列の取得
const std::vector<Property>& PropertyRegistry::GetProperties() const{
	return properties_;
}
