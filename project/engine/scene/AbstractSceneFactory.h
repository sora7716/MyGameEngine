#pragma once
#include "BaseScene.h"
#include <string>

/// <summary>
/// シーン工場(概念)
/// </summary>
class AbstractSceneFactory{
public://メンバ関数
	/// <summary>
	/// 仮想デストラクタ
	/// </summary>
	virtual ~AbstractSceneFactory();

	/// <summary>
	/// シーン生成
	/// </summary>
	/// <param name="sceneName">シーン名</param>
	/// <returns>シーン</returns>
	virtual BaseScene* CreateScene(const std::string& sceneName) = 0;
public://PassKey
	class ConstructorKey {
	private:
		ConstructorKey() = default;
		friend class Core;
	};

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PassKeyを受け取る</param>
	explicit AbstractSceneFactory(ConstructorKey);
};

