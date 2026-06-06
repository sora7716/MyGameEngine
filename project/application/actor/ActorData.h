#pragma once
#include "GameObject.h"
#include "ColliderData.h"
#include "ResourceData.h"
#include <memory>

//前方宣言
class Object3d;
class WireframeObject3d;

////描画時に必要な物
////object3d: オブジェクト3d
////hitBox: ヒットボックス
////material: マテリアル
//struct RenderObject {
//	std::unique_ptr<Object3d> object3d;
//	std::unique_ptr<WireframeObject3d> hitBox;
//	Material material;
//
//	/// <summary>
//	/// 生成
//	/// </summary>
//	/// <returns>レンダーオブジェクト</returns>
//	RenderObject& Create();
//
//	/// <summary>
//	/// マテリアルの初期化
//	/// </summary>
//	/// <returns>レンダーオブジェクト</returns>
//	RenderObject& InitializeMaterial();
//
//	/// <summary>
//	/// 作成
//	/// </summary>
//	/// <returns>右辺値のレンダーオブジェクト</returns>
//	RenderObject&& Build();
//};
//
////実体
//struct Entity {
//	Object3dInstance gameObject;
//	PhysicsData physicsData;
//	Vector3 hitBoxScale;
//	ColliderState colliderState;
//	Collider collider;
//};
//
////実体の塊
//struct EntityGroup {
//	std::vector<Entity>entity;
//	RenderObject renderObject;
//	std::string modelName;
//	int32_t objectCount;
//};
