#include "Mesh.h"
#include "DirectXBase.h"

//コンストラクタ
Mesh::Mesh() {
}

//デストラクタ
Mesh::~Mesh() {
}

//初期化
void Mesh::Initialize(DirectXBase* directXBase, const MeshData& meshData) {
	//DirectXの基盤部分を受け取る
	directXBase_ = directXBase;
	//メッシュデータを受け取る
	meshData_ = meshData;
	//頂点リソースの生成
	CreateVertexResource();
	//インデックスリソースの生成
	CreateIndexResource();
	//AABBの作成
	CreateAABB();
}

//描画
void Mesh::Draw(uint32_t objectCount) {
	//VertexBufferViewの設定
	directXBase_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
	directXBase_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);//IBVを設定
	//オブジェクト数が0より大きければ
	if (objectCount > 0) {
		//メッシュが空じゃなければ
		if (!meshData_.indices.empty()) {
			//描画
			directXBase_->GetCommandList()->DrawIndexedInstanced(UINT(meshData_.indices.size()), objectCount, 0, 0, 0);
		}
	}
}

//マテリアルインデックスの取得
uint32_t Mesh::GetMaterialIndex() {
	return meshData_.materialIndex;
}

//AABBの取得
const PrimitiveData::AABB& Mesh::GetAABB() const {
	// TODO: return ステートメントをここに挿入します
	return aabb_;
}

//頂点リソースの生成
void Mesh::CreateVertexResource() {
	//頂点リソースを生成
	vertexResource_ = directXBase_->CreateBufferResource(sizeof(VertexData) * meshData_.vertices.size());
	//VertexBufferViewを作成する(頂点バッファービュー)
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * meshData_.vertices.size());
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//頂点データをリソースにコピー
	std::memcpy(vertexData, meshData_.vertices.data(), sizeof(VertexData) * meshData_.vertices.size());
}

//インデックスリソースの生成
void Mesh::CreateIndexResource() {
	//インデックスのサイズが0だったら作らないようにする
	if (meshData_.indices.empty()) {
		return;
	}
	//Index用(3dGameObject)
	indexResource_ = directXBase_->CreateBufferResource(sizeof(uint32_t) * meshData_.indices.size());
	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * meshData_.indices.size());
	//インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	//インデックスリソースにデータを書き込む
	uint32_t* indexData = nullptr;
	//書き込むアドレスを取得
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	//インデックスデータをリソースにコピー
	std::memcpy(indexData, meshData_.indices.data(), sizeof(uint32_t) * meshData_.indices.size());
}

//AABBの生成
void Mesh::CreateAABB() {
	//頂点データが無ければ
	if (meshData_.vertices.empty()) {
		return;
	}

	//最小値の決める
	aabb_.min = {
		meshData_.vertices[0].position.x,
		meshData_.vertices[0].position.y,
		meshData_.vertices[0].position.z,
	};

	//最大値の決める
	aabb_.max = aabb_.min;

	//AABBを作成
	for (const VertexData& vertex : meshData_.vertices) {
		Vector3 vertexPosition = { vertex.position.x,vertex.position.y,vertex.position.z };

		//最小値を求める
		aabb_.min = aabb_.min.Min(vertexPosition);
		//最大値を求める
		aabb_.max = aabb_.max.Max(vertexPosition);
	}
}
