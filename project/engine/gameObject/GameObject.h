#pragma once
#include "RenderingData.h"
#include <string>

//タグ
enum class Tag {
	kPlayer,
	kJumpPad,
	kGround,
	kGoal,
	kNone
};

//ゲームオブジェクト
class GameObject {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameObject();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameObject();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="name"></param>
	void Initialize(const std::string& name = "GameObject");

	/// <summary>
	/// トランスフォームの取得
	/// </summary>
	/// <returns>トランスフォーム</returns>
	Transform& GetTransform();

	/// <summary>
	/// トランスフォームの取得
	/// </summary>
	/// <returns>トランスフォーム</returns>
	const Transform& GetTransform()const;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="isActive"></param>
	void SetIsActive(bool isActive);

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	bool IsActive()const;

	/// <summary>
	/// タグの設定
	/// </summary>
	/// <param name="tag">タグ</param>
	void SetTag(Tag tag);

	/// <summary>
	/// タグの取得
	/// </summary>
	/// <returns>タグ</returns>
	Tag GetTag()const;

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	const std::string& GetName()const;
private://メンバ変数
	std::string name_ = "\0";
	Transform transform_ = {};
	bool isActive_ = false;
	//bool isEnabled_ = false;
	Tag tag_ = Tag::kNone;
};