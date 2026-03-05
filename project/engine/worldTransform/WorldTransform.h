#pragma once
#include "engine/math/ResourceData.h"
#include "engine/math/RenderingData.h"
#include <string>
#include <wrl.h>
#include <d3d12.h>

//前方宣言
class DirectXBase;
class Camera;

//列挙型
enum class TransformMode :uint32_t {
	k3d,
	k2d,
	kBilboard,
	kDirectionToDirection
};
/// <summary>
/// ワールドトランスフォーム
/// </summary>
class WorldTransform {
private://エイリアステンプレート
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://構造体と列挙型
	//正規化デバイスするときに使用する
	struct ScreenArea {
		float left;
		float top;
		float right;
		float bottom;
	};
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	WorldTransform() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~WorldTransform() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="transformMode">トランスフォームのモード</param>
	void Initialize(DirectXBase* directXBase, TransformMode transformMode);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// スクリーンに表示する範囲
	/// </summary>
	/// <param name="screenArea">スクリーンエリア</param>
	void SetScreenArea(ScreenArea screenArea);

	/// <summary>
	/// カメラのセッター
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(Camera* camera);

	/// <summary>
	/// 親のセッター
	/// </summary>
	/// <param name="parent">親</param>
	void SetParent(const WorldTransform* parent);

	/// <summary>
	/// 親子付け
	/// </summary>
	/// <param name="parent">親</param>
	void Compose(const WorldTransform* parent);

	/// <summary>
	/// 親子関係を解除
	/// </summary>
	void Decompose();

	/// <summary>
	/// ワールド座標のセッター
	/// </summary>
	/// <param name="transform">ワールド座標</param>
	void SetTransformData(const TransformData& transform);

	/// <summary>
	/// ワールド座標のセッター(2D)
	/// </summary>
	/// <param name="transform2d">ワールド座標(2d)</param>
	void SetTransform2d(const Transform2dData& transform2d);

	/// <summary>
	/// スケールのセッター
	/// </summary>
	/// <param name="scale">スケール</param>
	void SetScale(const Vector3& scale);

	/// <summary>
	/// 回転のセッター
	/// </summary>
	/// <param name="rotate">回転</param>
	void SetRotate(const Vector3& rotate);

	/// <summary>
	/// 平行移動のセッター
	/// </summary>
	/// <param name="translate">平行移動</param>
	void SetTranslate(const Vector3& translate);
	
	/// <summary>
	/// ワールドマトリックスのセッター
	/// </summary>
	/// <param name="worldMatrix">ワールドマトリックス</param>
	void SetWorldMatrix(const Matrix4x4& worldMatrix);

	/// <summary>
	/// ノードのセッター
	/// </summary>
	/// <param name="node">ノード</param>
	void SetNode(const Node& node);

	/// <summary>
	/// ワールド行列のゲッター
	/// </summary>
	/// <returns>ワールド行列</returns>
	const Matrix4x4& GetWorldMatrix()const;

	/// <summary>
	/// スケールのゲッター
	/// </summary>
	/// <returns>スケール</returns>
	const Vector3& GetScale()const;

	/// <summary>
	/// 回転のゲッター
	/// </summary>
	/// <returns>回転</returns>
	const Quaternion& GetQuaternion()const;

	/// <summary>
	/// 平行移動のセッター
	/// </summary>
	/// <returns>平行移動</returns>
	const Vector3& GetTranslate()const;

	/// <summary>
	/// トランスフォームデータのゲッター
	/// </summary>
	/// <returns>トランスフォームデータ</returns>
	const TransformData& GetTransform()const;

	/// <summary>
	/// カメラのゲッター
	/// </summary>
	/// <returns>カメラ</returns>
	Camera* GetCamera();

	/// <summary>
	/// ワールド座標のゲッター
	/// </summary>
	/// <returns>ワールド座標</returns>
	Vector3 GetWorldPos();

	/// <summary>
	/// FromPosとToPosのセッター
	/// </summary>
	/// <param name="fromPos">現在の位置</param>
	/// <param name="toPos">目的の位置</param>
	void SetFromAndToPos(const Vector3& fromPos, const Vector3& toPos);
private://メンバ関数
	/// <summary>
	/// 座標変換行列リソースの生成
	/// </summary>
	void CreateTransformationMatrixResource();

	/// <summary>
	/// 座標の更新
	/// </summary>
	void UpdateTransform();

	/// <summary>
	/// 座標の更新(向きたい方向に向かせる)
	/// </summary>
	void UpdateTrasformDirectionToDirection();

	/// <summary>
	/// 座標の更新(2次元)
	/// </summary>
	void UpdateTransform2d();

	/// <summary>
	/// ビルボード行列での更新
	/// </summary>
	void UpdateTransformBillboard();
private://メンバ関数テーブル
	//座標の更新をまとめた
	static void (WorldTransform::* UpdateTransformTable[])();
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//ワールドビュープロジェクションのリソース
	ComPtr<ID3D12Resource>wvpResource_ = nullptr;
	//ワールドビュープロジェクションのデータ
	TransformationMatrix* wvpData_ = nullptr;
	//カメラ
	Camera* camera_ = nullptr;
	//切り取る範囲
	ScreenArea screenArea_ = {};
	//トランスフォームモード
	TransformMode transformMode_ = TransformMode::k3d;
	//ワールド座標
	TransformData transform_ = {};
	//ワールド行列
	Matrix4x4 worldMatrix_ = {};
	//親
	const WorldTransform* parent_ = nullptr;
	//ノード
	Node node_ = {};

	//fromPos
	Vector3 fromPos_ = {};
	//toPos
	Vector3 toPos_ = {};
};


