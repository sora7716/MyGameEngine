#pragma once
#include "AbstractSceneFactory.h"

/// <summary>
/// このゲーム用のシーン工場
/// </summary>
class SceneFactory :public AbstractSceneFactory {
public://メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~SceneFactory()override;

	/// <summary>
	/// シーンの生成
	/// </summary>
	/// <param name="sceneName">シーン名</param>
	/// <returns>シーン</returns>
	BaseScene* CreateScene(const std::string& sceneName)override;
public://PrassKey
	class ConstructorKey {
	private:
		ConstructorKey() = default;
		friend class Core;
	};

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PrassKeyを受け取る</param>
	explicit SceneFactory(AbstractSceneFactory::ConstructorKey);
};

