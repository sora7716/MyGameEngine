#define NOMINMAX
#include "Model.h"
#include "DirectXBase.h"
#include "ModelCommon.h"
#include "Mesh.h"
#include "TextureManager.h"
#include "Logger.h"
#include "algorithms/Math.h"
#include <format>
#include <map>
#include <unordered_map>
#include <queue>
#include <cassert>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
/// <summary>
/// assimpのノードを読み取る
/// </summary>
/// <param name="node">assimpのノード</param>
/// <returns>ノード</returns>
static Node ReadNode(aiNode* node) {
	Node result;
	//nodeのlocalMatrixを取得
	aiMatrix4x4 aiLocalMatrix = node->mTransformation;
	//行列を転置
	aiLocalMatrix.Transpose();

	//aiMatrix4x4からMatrix4x4へ変換
	for (int32_t i = 0; i < 4; i++) {
		for (int32_t j = 0; j < 4; j++) {
			result.localMatrix.m[i][j] = aiLocalMatrix[i][j];
		}
	}
	//Nodeの名前を取得
	result.name = node->mName.C_Str();
	//子の数だけ
	result.children.resize(node->mNumChildren);

	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; childIndex++) {
		//再帰的に呼んで階層構造を作っていく
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}

//コンストラクタ
Model::Model() {
}

//デストラクタ
Model::~Model() {
}

//モデルの生成(ファイルを読み込んでの)
std::unique_ptr<Model>Model::CreateFromModel(ModelCommon* modelCommon, const std::string& storedFilePath, const std::string& filename) {
	//インスタンスの生成
	std::unique_ptr<Model>instance = std::make_unique<Model>();
	//初期化
	instance->Initialize(modelCommon);
	//モデルの生成
	instance->CreateFromModel(storedFilePath, filename);
	return instance;
}

//モデルの生成(キューブ)
std::unique_ptr<Model> Model::CreateCube(ModelCommon* modelCommon) {
	//インスタンスの生成
	std::unique_ptr<Model>instance = std::make_unique<Model>();
	//初期化
	instance->Initialize(modelCommon);
	//モデルの生成
	instance->CreateCube();
	return instance;
}

//モデルの生成(モデルデータ)
std::unique_ptr<Model> Model::CreateModelFromModelData(ModelCommon* modelCommon, const ModelData& modelData) {
	//インスタンスの生成
	std::unique_ptr<Model>instance = std::make_unique<Model>();
	//初期化
	instance->Initialize(modelCommon);
	//モデルの生成
	instance->CreateModelFromModelData(modelData);
	return instance;
}

//初期化
void Model::Initialize(ModelCommon* modelCommon) {
	//ModelCommonのポインタを引数からメンバ変数を記録する
	modelCommon_ = modelCommon;
	//DirectXの基盤部分を受け取る
	directXBase_ = modelCommon_->GetDirectXBase();
}

//メッシュの再構成
void Model::RebuildMeshes(const std::vector<MeshData>& meshes) {
	//メッシュデータのクリア
	if (!meshes_.empty()) {
		meshes_.clear();
	}
	//受け取ったメッシュデータに書き換え
	modelData_.mesheDatas = meshes;
	//マテリアルが存在するか
	if (!modelData_.material.empty()) {
		for (uint32_t i = 0; i < modelData_.mesheDatas.size(); i++) {
			if (modelData_.mesheDatas[i].materialIndex >= modelData_.material.size()) {
				modelData_.mesheDatas[i].materialIndex = 0;
			}
		}
	} else {
		//マテリアルが存在しなかった場合
		MaterialData material;
#ifdef _DEBUG
		material.textureFilePath = "engine/resources/textures/magenta1x1.png";
#else
		material.textureFilePath = "engine/resources/textures/white1x1.png";
#endif // _DEBUG
		modelData_.material.push_back(material);
	}
	//メッシュを構築
	BuildMesh();
	//マテリアルリソースとポインタのサイズ設定
	materialResources_.resize(modelData_.material.size());
	materialPtrs_.resize(modelData_.material.size());
}

//頂点を合成する
std::vector<MeshData> Model::VertexClustering(float rate, float min, float max) {
	//メッシュ
	std::vector<MeshData>baseMeshes = modelData_.mesheDatas;

	//新しいメッシュ
	std::vector<MeshData>newMeshes;

	//割合が1.0fより大きかったら
	if (rate >= 1.0f) {
		return baseMeshes;
	}

	//割合が0.0fより小さかった場合
	if (rate < 0.0f) {
		Logger::ConsolePrintf("[Model::VertexClustering] rate is 0.0f or less. Invalid rate. Return original meshes.\n");
		return baseMeshes;
	}

	//割合をもとに取得したい頂点数を出す
	for (const MeshData& meshData : baseMeshes) {
		uint32_t goalVertexCount = static_cast<uint32_t>(static_cast<float>(meshData.vertices.size()) * rate);
		//グリッドサイズ
		float minGridSize = min;//最小値
		float maxGridSize = max;//最大値

		//目標の頂点数に一番違いメッシュ
		MeshData bestMesh = meshData;
		uint32_t bestMeshVertexCount = static_cast<uint32_t>(bestMesh.vertices.size());
		float gridSize = 0;
		//試行回数
		const uint32_t kTrialCount = 20;
		//二分探索
		for (uint32_t i = 0; i < kTrialCount; i++) {
			//gridSizeはminとmaxの中間
			gridSize = (minGridSize + maxGridSize) / 2.0f;

			//元メッシュをgridSizeでクラスタリング
			MeshData trialMesh = VertexClusteringByGridSize(meshData, gridSize);
			//クラスタリングしたメッシュの頂点数を取得
			uint32_t trialMeshVertexCount = static_cast<uint32_t>(trialMesh.vertices.size());

			//目標の頂点数より差分が小さいほうのメッシュを入れる
			if (std::fabs(static_cast<float>(goalVertexCount) - static_cast<float>(bestMeshVertexCount)) > std::fabs(static_cast<float>(goalVertexCount) - static_cast<float>(trialMeshVertexCount))) {
				//ベストメッシュの置き換え
				bestMesh = trialMesh;
				//頂点の数の記録
				bestMeshVertexCount = static_cast<uint32_t>(bestMesh.vertices.size());
			}

			//範囲を狭めていく
			if (trialMeshVertexCount > goalVertexCount) {
				minGridSize = gridSize;
			} else if (trialMeshVertexCount < goalVertexCount) {
				maxGridSize = gridSize;
			}
		}
		//新しいメッシュに追加する
		newMeshes.push_back(bestMesh);
	}

	return newMeshes;
}

//uint32_tの変数を二つ切り詰めてuint64_tの検索キーを作成
uint64_t MakeEdgeKey(uint32_t a, uint32_t b) {
	std::array<uint32_t, 2>edge = {};
	edge = { std::min(a,b),std::max(a,b) };
	return static_cast<uint64_t>(edge[0]) << 32 | static_cast<uint64_t>(edge[1]);
}

//辺縮約
std::vector<MeshData> Model::EdgeCollapse(float rate) {
	//メッシュデータを記録
	std::vector<MeshData> baseMeshDatas = modelData_.mesheDatas;
	//割合が1より大きかったら辺縮約しない
	if (rate >= 1.0f) {
		return baseMeshDatas;
	}
	//辺の消しやすさのスコアを作成
	struct EdgeCandidate {
		std::array<uint32_t, 2>edgeIndices;
		float score;
		uint32_t useCount;
		std::array<uint32_t, 2>edgeIndexVersions;
	};

	//スコアが小さいものから出す
	struct EdgeScoreCompare {
		bool operator()(const EdgeCandidate& a, const EdgeCandidate& b)const {
			return a.score > b.score;
		}
	};

	//各メッシュごとに処理をする
	for (MeshData& baseMeshData : baseMeshDatas) {
		//インデックスのサイズが3の倍数じゃなかった場合
		if (baseMeshData.indices.size() % 3 != 0) {
			continue;
		}
		//現在の頂点数
		uint32_t currentVertexCount = static_cast<uint32_t>(baseMeshData.vertices.size());
		//最終的な頂点数
		const uint32_t goalVertexCount = static_cast<uint32_t>(static_cast<float>(baseMeshData.vertices.size()) * rate);
		//優先度付きキュー(入れるデータ型、内部で使う入れ物、並び順のルール)
		std::priority_queue<EdgeCandidate, std::vector<EdgeCandidate>, EdgeScoreCompare>edgeQueue;
		//辺の一覧表(検索キーを辺の組み合わせ、valueをそのキーの出現回数)
		std::unordered_map<uint64_t, uint32_t>edgeList;
		//頂点のバージョン
		std::vector<uint32_t>vertexVersion(baseMeshData.vertices.size(), 0);

		while (currentVertexCount > goalVertexCount) {
			//Collapse開始する前の頂点数を記録
			uint32_t startVertexCount = static_cast<uint32_t>(baseMeshData.vertices.size());
			//辺の一覧表のリセット
			edgeList.clear();
			//辺のキューをリセット
			while (!edgeQueue.empty()) {
				edgeQueue.pop();
			}

			for (uint32_t i = 0; i < baseMeshData.indices.size(); i += 3) {
				//三角形を作る
				uint32_t a = baseMeshData.indices[i];
				uint32_t b = baseMeshData.indices[i + 1];
				uint32_t c = baseMeshData.indices[i + 2];
				//出現回数を増やしながら辺一覧に追加
				edgeList[MakeEdgeKey(a, b)]++;
				edgeList[MakeEdgeKey(b, c)]++;
				edgeList[MakeEdgeKey(c, a)]++;
			}

			//境界っぽい辺があるかを判断するフラグ
			std::vector<uint8_t>isBoundaryVertices(baseMeshData.vertices.size(), false);

			//共通近傍チェック用の配列作成
			//各頂点ごとに隣り合う頂点を取得
			std::vector<std::vector<uint32_t>>neighbors;
			neighbors.resize(baseMeshData.vertices.size());
			//三角形を追加
			for (uint32_t i = 0; i < baseMeshData.indices.size(); i += 3) {
				uint32_t a = baseMeshData.indices[i];
				uint32_t b = baseMeshData.indices[i + 1];
				uint32_t c = baseMeshData.indices[i + 2];
				neighbors[a].push_back(b);
				neighbors[a].push_back(c);

				neighbors[b].push_back(a);
				neighbors[b].push_back(c);

				neighbors[c].push_back(a);
				neighbors[c].push_back(b);
			}

			//重複を削除
			for (std::vector<uint32_t>& neighbor : neighbors) {
				//ソート
				std::sort(neighbor.begin(), neighbor.end());

				//重複を削除
				neighbor.erase(std::unique(neighbor.begin(), neighbor.end()), neighbor.end());
			}

			//境界頂点をtrueにする
			for (const auto& edge : edgeList) {
				//辺の検索キーを取得
				uint64_t edgeKey = edge.first;
				uint32_t v0 = static_cast<uint32_t>(edgeKey >> 32);
				uint32_t v1 = static_cast<uint32_t>(edgeKey & UINT32_MAX);

				//普通の内部辺以外の場合
				if (edge.second != 2) {
					isBoundaryVertices[v0] = true;
					isBoundaryVertices[v1] = true;
				}
			}

			//スコア付けをしていく
			for (const auto& edge : edgeList) {
				//辺の検索キーを取得
				uint64_t edgeKey = edge.first;
				uint32_t v0 = static_cast<uint32_t>(edgeKey >> 32);
				uint32_t v1 = static_cast<uint32_t>(edgeKey & UINT32_MAX);

				//普通の内部辺以外の場合
				if (edge.second != 2) {
					continue;
				}

				//境界頂点がtrueだったらスキップする
				if (isBoundaryVertices[v0] || isBoundaryVertices[v1]) {
					continue;
				}

				//共通近傍チェック
				//v0-v1で共有している頂点数のカウント
				uint32_t sharedNeighborCount = 0;
				for (uint32_t i = 0; i < neighbors[v0].size(); i++) {
					//共有している点が2より大きくなったら
					if (sharedNeighborCount > 2) {
						break;
					}
					for (uint32_t j = 0; j < neighbors[v1].size(); j++) {
						//共有している頂点があったら
						if (neighbors[v0][i] == neighbors[v1][j]) {
							sharedNeighborCount++;
							break;
						}
					}
				}

				//共有している頂点が2つでなければスキップ
				if (sharedNeighborCount != 2) {
					continue;
				}

				//辺の検索キーを取得
				std::array<uint32_t, 2>edgeIndices = { v0,v1 };
				//辺のスコアを記録
				EdgeCandidate edgeCandidate = {};

				//インデックスの記録
				edgeCandidate.edgeIndices = edgeIndices;

				//比較用のスコアを取得
				Vector3 p0 = {
					baseMeshData.vertices[edgeIndices[0]].position.x,
					baseMeshData.vertices[edgeIndices[0]].position.y,
					baseMeshData.vertices[edgeIndices[0]].position.z,
				};
				Vector3 p1 = {
					baseMeshData.vertices[edgeIndices[1]].position.x,
					baseMeshData.vertices[edgeIndices[1]].position.y,
					baseMeshData.vertices[edgeIndices[1]].position.z,
				};
				//距離
				Vector3 distance = p1 - p0;
				edgeCandidate.score = distance.LengthSquared();

				//出現回数の保持
				edgeCandidate.useCount = edge.second;

				//辺のバージョンを保存
				edgeCandidate.edgeIndexVersions[0] = vertexVersion[v0];
				edgeCandidate.edgeIndexVersions[1] = vertexVersion[v1];

				//キューに追加
				edgeQueue.push(edgeCandidate);

			}

			//辺のスコア表が空だったら
			if (edgeQueue.empty()) {
				break;
			}

			//Collapseしたい数
			uint32_t batchCount = 50;
			float batchRate = 0.01f;
			//割合ごとにCollapseしたい数と割合も変更
			if (rate <= 0.25f) {
				batchCount = 1000;
				batchRate = 0.2f;
			} else if (rate <= 0.5f) {
				batchCount = 800;
				batchRate = 0.1f;
			} else if (rate <= 0.8f) {
				batchCount = 200;
				batchRate = 0.02f;
			}
			//現在の頂点数と目標の頂点数の差分
			uint32_t remainingCount = currentVertexCount - goalVertexCount;
			//現在の頂点数の何割かを取得
			uint32_t rateBasedBatchCount = static_cast<uint32_t>(static_cast<float>(currentVertexCount) * batchRate);
			//もし0以下になっていたら
			if (rateBasedBatchCount == 0) {
				rateBasedBatchCount = 1;
			}
			//Collapseできる数
			uint32_t candidateCount = std::min({ remainingCount, rateBasedBatchCount, batchCount });
			//uint32_t candidateCount = 1;
			std::vector<EdgeCandidate> minCandidates;
			minCandidates.reserve(candidateCount);//容量の確保
			//候補に入れたかどうかのフラグ表
			std::vector<uint8_t>isUseThisPass(currentVertexCount, false);
			//法線を考慮する
			float normalDotThreshold = 0.7f;
			while (minCandidates.size() < candidateCount) {
				//キューが空になったら
				if (edgeQueue.empty()) {
					break;
				}

				//一番Scoreが小さい辺を選ぶ
				EdgeCandidate minCandidate = edgeQueue.top();
				edgeQueue.pop();

				//法線を比べる
				Vector3 normal0 = baseMeshData.vertices[minCandidate.edgeIndices[0]].normal;
				Vector3 normal1 = baseMeshData.vertices[minCandidate.edgeIndices[1]].normal;
				float normalDot = normal0.Dot(normal1);
				//法線の内積がnormalDotThresholdより小さければ飛ばす(同じ方向を見ていないってことなので)
				if (normalDot < normalDotThreshold) {
					continue;
				}

				//辺の点が候補に選ばれたか
				//edgeIndices[0]またはedgeIndices[1]どちらか候補に挙がってたか
				if (isUseThisPass[minCandidate.edgeIndices[0]] || isUseThisPass[minCandidate.edgeIndices[1]]) {
					continue;
				}
				//edgeIndices[0]またはedgeIndices[1]が候補に挙がってなかった場合
				isUseThisPass[minCandidate.edgeIndices[0]] = true;
				isUseThisPass[minCandidate.edgeIndices[1]] = true;

				//候補に追加
				minCandidates.push_back(minCandidate);
			}

			//Collapseする候補が空だったら
			if (minCandidates.empty()) {
				break;
			}

			//CollapseしたIndexを保存する対応表
			std::vector<uint32_t>collapseTo(currentVertexCount, UINT32_MAX);
			for (const EdgeCandidate& minCandidate : minCandidates) {
				//Collapseする
				uint32_t v0 = minCandidate.edgeIndices[0];
				uint32_t v1 = minCandidate.edgeIndices[1];
				//v0位置をv1とV0の中点にする
				Vector4 vertexPos0 = baseMeshData.vertices[v0].position;
				Vector4 vertexPos1 = baseMeshData.vertices[v1].position;
				Vector4 mid = (vertexPos0 + vertexPos1) / 2.0f;
				mid.w = 1.0f;
				baseMeshData.vertices[v0].position = mid;
				//対応用に追加
				collapseTo[v1] = v0;
				//頂点バージョンの更新
				vertexVersion[v0]++;
				vertexVersion[v1]++;
			}

			//対応表からインデックスを適応
			for (uint32_t& index : baseMeshData.indices) {
				//UINT_MAXじゃなければindexに追加
				if (collapseTo[index] != UINT32_MAX) {
					index = collapseTo[index];
				}
			}

			//インデックスの張替え
			std::vector<uint32_t>collapseToNewIndices;
			collapseToNewIndices.reserve(baseMeshData.indices.size());
			for (uint32_t i = 0; i < baseMeshData.indices.size(); i += 3) {
				uint32_t a = baseMeshData.indices[i];
				uint32_t b = baseMeshData.indices[i + 1];
				uint32_t c = baseMeshData.indices[i + 2];

				//三角形が作れない場合は省く
				if (a == b) {
					continue;
				} else if (a == c) {
					continue;
				} else if (b == c) {
					continue;
				}

				//新しいインデックスを追加
				collapseToNewIndices.push_back(a);
				collapseToNewIndices.push_back(b);
				collapseToNewIndices.push_back(c);
			}

			//Collapseした後のインデックスが空だった場合
			if (collapseToNewIndices.empty()) {
				break;
			}

			//インデックスの更新
			baseMeshData.indices = collapseToNewIndices;

			//使用されている頂点のフラグ
			std::vector<uint8_t>isUseVertices(currentVertexCount, false);
			//昔のインデックスを新しいインデックスに変更する対応表
			std::vector<uint32_t>oldToNewIndices(currentVertexCount, UINT32_MAX);
			//新しい頂点
			std::vector<VertexData>newVertices;
			newVertices.reserve(isUseVertices.size());
			//新しい頂点のバージョン
			std::vector<uint32_t>newVerticesVersion;
			newVerticesVersion.reserve(isUseVertices.size());
			//頂点の配列を使用されている奴だけにする
			for (uint32_t index : baseMeshData.indices) {
				//使用されている頂点をtrueに
				isUseVertices[index] = true;
			}

			//新しい頂点を生成
			uint32_t newIndex = 0;
			for (uint32_t oldIndex = 0; oldIndex < isUseVertices.size(); oldIndex++) {
				if (!isUseVertices[oldIndex]) {
					continue;
				}
				//新しい頂点を挿入
				newVertices.push_back(baseMeshData.vertices[oldIndex]);
				//新しい頂点の番号に合わせてバージョンも挿入
				newVerticesVersion.push_back(vertexVersion[oldIndex]);
				//昔のインデックスのところに新しいインデックスを代入
				oldToNewIndices[oldIndex] = newIndex;
				newIndex++;
			}
			//インデックスの張替え
			std::vector<uint32_t>newIndices;
			newIndices.reserve(baseMeshData.indices.size());
			for (uint32_t i = 0; i < baseMeshData.indices.size(); i += 3) {
				uint32_t a = oldToNewIndices[baseMeshData.indices[i]];
				uint32_t b = oldToNewIndices[baseMeshData.indices[i + 1]];
				uint32_t c = oldToNewIndices[baseMeshData.indices[i + 2]];
				//a,b,cのどれかがUINT32_MAXになっていたらスキップ
				if (a == UINT32_MAX || b == UINT32_MAX || c == UINT32_MAX) {
					continue;
				}

				//三角形が作れない場合は省く
				if (a == b) {
					continue;
				} else if (a == c) {
					continue;
				} else if (b == c) {
					continue;
				}

				//新しいインデックスを追加
				newIndices.push_back(a);
				newIndices.push_back(b);
				newIndices.push_back(c);
			}

			//新しく作ったインデックスが空だったら
			if (newIndices.empty()) {
				break;
			}

			//頂点データの更新
			baseMeshData.vertices = newVertices;
			//頂点のバージョンの更新
			vertexVersion = newVerticesVersion;
			//インデックスデータの更新
			baseMeshData.indices = newIndices;

			//現在の頂点の数を保存
			currentVertexCount = static_cast<uint32_t>(baseMeshData.vertices.size());

			//Collapseした後の頂点とする前の頂点を以上になっていたら
			if (currentVertexCount >= startVertexCount) {
				break;
			}
		}
	}
	return baseMeshDatas;
}

//描画
void Model::Draw(uint32_t objectCount) {
	//リムライトのCBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(7, rimLightResource_->GetGPUVirtualAddress());
	//メッシュの描画
	for (std::unique_ptr<Mesh>& mesh : meshes_) {
		uint32_t materialIndex = mesh->GetMaterialIndex();

		//マテリアルCBufferの場所を設定
		directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[materialIndex]->GetGPUVirtualAddress());

		MaterialData& materialData = modelData_.material[materialIndex];

		//テクスチャをセット
		directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(2, modelCommon_->GetTextureManager()->GetSRVHandleGPU(materialData.textureFilePath));
		mesh->Draw(objectCount);
	}
}

//uv変換
void Model::UVTransform(uint32_t index, Transform2d uvTransform) {
	materialPtrs_[index]->uvMatrix = Rendering::MakeUVAffineMatrix(uvTransform);
}

// 色を変更
void Model::SetColor(uint32_t index, const Vector4& color) {
	materialPtrs_[index]->color = color;
}

//テクスチャの変更
void Model::SetTexture(uint32_t materialIndex, const std::string& imageName) {
	modelData_.material[materialIndex].textureFilePath = "engine/resources/textures/" + imageName;
	modelCommon_->GetTextureManager()->LoadTexture(modelData_.material[materialIndex].textureFilePath);
}

//色を取得
const Vector4& Model::GetColor(uint32_t index) const {
	// TODO: return ステートメントをここに挿入します
	return materialPtrs_[index]->color;
}

//モデルデータのゲッター
const ModelData& Model::GetModelData() const {
	// TODO: return ステートメントをここに挿入します
	return modelData_;
}

//.mtlファイルの読み取り	
MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	//1.中で必要となる変数の宣言
	MaterialData materialData;//構築するMaterialData
	std::string line;//ファイルから読んだ1行を格納するもの
	//2.ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
	assert(file.is_open());//とりあえず開かなかったら止める
	//3.実際にファイルを読み込み、MaterialDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;
		//identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	//4.MaterialDataを返す
	return materialData;
}

//モデルファイルの読み込み
ModelData Model::LoadModelFile(const std::string& directoryPath, const std::string& storedFilePath, const std::string& filename) {
	//構築するModelData
	ModelData modelData;

	//assimpでobjファイルを読み込む
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + storedFilePath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());//メッシュがない場合は終了

	//meshを解析する
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++) {
		aiMesh* assimpMesh = scene->mMeshes[meshIndex];
		assert(assimpMesh->HasNormals());//法線がないメッシュは未対応
		assert(assimpMesh->HasTextureCoords(0));//Texcoordがないメッシュは未対応

		//メッシュデータ
		MeshData meshData;
		meshData.materialIndex = assimpMesh->mMaterialIndex;

		//頂点を見る
		for (uint32_t vertexIndex = 0; vertexIndex < assimpMesh->mNumVertices; vertexIndex++) {
			aiVector3D& position = assimpMesh->mVertices[vertexIndex];
			aiVector3D& normal = assimpMesh->mNormals[vertexIndex];
			aiVector3D& texcoord = assimpMesh->mTextureCoords[0][vertexIndex];

			VertexData vertex = {};
			vertex.position = { position.x,position.y,position.z,1.0f };
			vertex.normal = { normal.x,normal.y,normal.z };
			vertex.texcoord = { texcoord.x, texcoord.y };

			//右手座標系から見だりて座標系に直してる
			vertex.position.z *= -1.0f;
			vertex.normal.z *= -1.0f;

			meshData.vertices.push_back(vertex);
		}

		//インデックスを見る
		for (uint32_t faceIndex = 0; faceIndex < assimpMesh->mNumFaces; faceIndex++) {
			aiFace& face = assimpMesh->mFaces[faceIndex];
			//三角形以外は未対応
			assert(face.mNumIndices == 3);

			meshData.indices.push_back(face.mIndices[0]);
			meshData.indices.push_back(face.mIndices[1]);
			meshData.indices.push_back(face.mIndices[2]);
		}

		//モデルデータにメッシュデータを移動
		modelData.mesheDatas.push_back(std::move(meshData));
	}

	//RootNodeの解析
	modelData.rootNode = ReadNode(scene->mRootNode);

	//materialを解析
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; materialIndex++) {
		aiMaterial* material = scene->mMaterials[materialIndex];

		//マテリアルデータ
		MaterialData materialData;

		if (material->GetTextureCount(aiTextureType_BASE_COLOR) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_BASE_COLOR, 0, &textureFilePath);

			materialData.textureFilePath = directoryPath + "/" + storedFilePath + "/" + textureFilePath.C_Str();
		} else if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			materialData.textureFilePath = directoryPath + "/" + storedFilePath + "/" + textureFilePath.C_Str();
		}

		//モデルデータのマテリアルにマテリアルデータを移動
		modelData.material.push_back(std::move(materialData));
	}

	return modelData;
}

//ライティングの設定
void Model::SetIsLighting(uint32_t index, bool isLighting) {
	materialPtrs_[index]->enableLighting = isLighting;
}

//輝度の設定
void Model::SetShininess(uint32_t index, float shininess) {
	materialPtrs_[index]->shininess = shininess;
}

//リムライトのセッター
void Model::SetRimLight(const RimLight& rimLight) {
	rimLightPtr_->color = rimLight.color;
	rimLightPtr_->outLinePower = rimLight.outLinePower;
	rimLightPtr_->power = rimLight.power;
	rimLightPtr_->softness = rimLight.softness;
	rimLightPtr_->enableRimLighting = rimLight.enableRimLighting;
}

//メッシュたちのゲッター
const std::vector<std::unique_ptr<Mesh>>& Model::GetMeshes() const {
	return meshes_;
}

//モデルの共通部分の取得
ModelCommon* Model::GetModelCommon() {
	return modelCommon_;
}

//マテリアルリソースの生成
void Model::CreateMaterialResource() {
	for (uint32_t i = 0; i < modelData_.material.size(); i++) {
		//マテリアル用のリソースを作る
		materialResources_[i] = directXBase_->CreateBufferResource(sizeof(Material));
		//書き込むためのアドレスを取得
		materialResources_[i]->Map(0, nullptr, reinterpret_cast<void**>(&materialPtrs_[i]));
		//色を書き込む
		materialPtrs_[i]->color = { 1.0f, 1.0f, 1.0f, 1.0f };
		materialPtrs_[i]->enableLighting = true;
		materialPtrs_[i]->uvMatrix = Matrix4x4::Identity4x4();
		materialPtrs_[i]->shininess = 10.0f;
	}
}

//リムライトのリソースを生成
void Model::CreateRimLightResource() {
	//マテリアル用のリソースを作る
	rimLightResource_ = directXBase_->CreateBufferResource(sizeof(RimLight));
	//書き込むためのアドレスを取得
	rimLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&rimLightPtr_));
	//色を書き込む
	rimLightPtr_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	rimLightPtr_->outLinePower = 0.1f;
	rimLightPtr_->power = 0.1f;
	rimLightPtr_->softness = 5.0f;
	rimLightPtr_->enableRimLighting = false;
}

//メッシュの構築
void Model::BuildMesh() {
	//メッシュの生成と初期化
	meshes_.reserve(modelData_.mesheDatas.size());
	for (const MeshData& meshData : modelData_.mesheDatas) {
		std::unique_ptr<Mesh>mesh = std::make_unique<Mesh>();
		mesh->Initialize(directXBase_, meshData);
		meshes_.push_back(std::move(mesh));
	}
}

//キューブの作成
MeshData Model::MakeCubeData() {
	//面のデータ
	struct FaceData {
		Vector3 normal;
		Vector4 position[4];
	};

	//どこの面
	enum FaceType :uint32_t {
		kFront,
		kBack,
		kRight,
		kLeft,
		kTop,
		kBottom,
		kFaceCount
	};

	//頂点の場所
	enum FaceRect :uint32_t {
		kLeftUp,
		kRightUp,
		kLeftBottom,
		kRightBottom,
		kFaceRectCount
	};

	//メッシュデータの頂点とインデックスのサイズ決定
	MeshData mesh = {};
	const uint32_t kVertexCount = 24;
	const uint32_t kIndexCount = 36;
	mesh.vertices.resize(24);
	mesh.indices.resize(36);

	//UV座標
	Vector2 uv[kFaceRectCount] = {};
	uv[kLeftUp] = { 0.0f,0.0f };//左上
	uv[kRightUp] = { 1.0f,0.0f };//右上
	uv[kLeftBottom] = { 0.0f,1.0f };//左下
	uv[kRightBottom] = { 1.0f,1.0f };//右下

	//法線と位置
	FaceData face[kFaceCount]{};

	//正面
	face[kFront].normal = { 0.0f, 0.0f, 1.0f };
	face[kFront].position[kLeftUp] = { -1.0f, 1.0f, 1.0f, 1.0f };
	face[kFront].position[kRightUp] = { 1.0f, 1.0f, 1.0f, 1.0f };
	face[kFront].position[kLeftBottom] = { -1.0f, -1.0f, 1.0f, 1.0f };
	face[kFront].position[kRightBottom] = { 1.0f, -1.0f, 1.0f, 1.0f };

	//背面
	face[kBack].normal = { 0.0f, 0.0f, 1.0f };
	face[kBack].position[kLeftUp] = { -1.0f, 1.0f, -1.0f, 1.0f };
	face[kBack].position[kRightUp] = { 1.0f, 1.0f, -1.0f, 1.0f };
	face[kBack].position[kLeftBottom] = { -1.0f, -1.0f, -1.0f, 1.0f };
	face[kBack].position[kRightBottom] = { 1.0f, -1.0f, -1.0f, 1.0f };

	//右面
	face[kRight].normal = { 1.0f, 0.0f, 0.0f };
	face[kRight].position[kLeftUp] = { 1.0f, -1.0f, 1.0f, 1.0f };
	face[kRight].position[kRightUp] = { 1.0f, 1.0f, 1.0f, 1.0f };
	face[kRight].position[kLeftBottom] = { 1.0f, -1.0f, -1.0f, 1.0f };
	face[kRight].position[kRightBottom] = { 1.0f, 1.0f, -1.0f, 1.0f };

	//左面
	face[kLeft].normal = { -1.0f, 0.0f, 0.0f };
	face[kLeft].position[kLeftUp] = { -1.0f, 1.0f, 1.0f, 1.0f };
	face[kLeft].position[kRightUp] = { -1.0f, 1.0f, -1.0f, 1.0f };
	face[kLeft].position[kLeftBottom] = { -1.0f, -1.0f, 1.0f, 1.0f };
	face[kLeft].position[kRightBottom] = { -1.0f, -1.0f, -1.0f, 1.0f };

	//上面
	face[kTop].normal = { 0.0f, 1.0f, 0.0f };
	face[kTop].position[kLeftUp] = { -1.0f, 1.0f, 1.0f, 1.0f };
	face[kTop].position[kRightUp] = { 1.0f, 1.0f, 1.0f, 1.0f };
	face[kTop].position[kLeftBottom] = { -1.0f, 1.0f, -1.0f, 1.0f };
	face[kTop].position[kRightBottom] = { 1.0f, 1.0f, -1.0f, 1.0f };

	//下面
	face[kBottom].normal = { 0.0f, -1.0f, 0.0f };
	face[kBottom].position[kLeftUp] = { -1.0f, -1.0f, 1.0f, 1.0f };
	face[kBottom].position[kRightUp] = { 1.0f, -1.0f, 1.0f, 1.0f };
	face[kBottom].position[kLeftBottom] = { -1.0f, -1.0f, -1.0f, 1.0f };
	face[kBottom].position[kRightBottom] = { 1.0f, -1.0f, -1.0f, 1.0f };

	//頂点データの入力
	for (uint32_t faceIndex = 0; faceIndex < kFaceCount; faceIndex++) {
		for (uint32_t faceRectIndex = 0; faceRectIndex < kFaceRectCount; faceRectIndex++) {
			uint32_t index = faceIndex * 4 + faceRectIndex;
			mesh.vertices[index] = {
				.position = face[faceIndex].position[faceRectIndex],
				.texcoord = uv[faceRectIndex],
				.normal = face[faceIndex].normal,
			};
		}
	}

	// Z+  正面
	mesh.indices[0] = 0;
	mesh.indices[1] = 2;
	mesh.indices[2] = 1;
	mesh.indices[3] = 2;
	mesh.indices[4] = 3;
	mesh.indices[5] = 1;

	// Z-  背面
	mesh.indices[6] = 4;
	mesh.indices[7] = 5;
	mesh.indices[8] = 6;
	mesh.indices[9] = 6;
	mesh.indices[10] = 5;
	mesh.indices[11] = 7;

	// X+  右面
	mesh.indices[12] = 8;
	mesh.indices[13] = 10;
	mesh.indices[14] = 9;
	mesh.indices[15] = 10;
	mesh.indices[16] = 11;
	mesh.indices[17] = 9;

	// X-  左面
	mesh.indices[18] = 12;
	mesh.indices[19] = 13;
	mesh.indices[20] = 14;
	mesh.indices[21] = 14;
	mesh.indices[22] = 13;
	mesh.indices[23] = 15;

	// Y+  上面
	mesh.indices[24] = 16;
	mesh.indices[25] = 17;
	mesh.indices[26] = 18;
	mesh.indices[27] = 18;
	mesh.indices[28] = 17;
	mesh.indices[29] = 19;

	// Y-  下面
	mesh.indices[30] = 20;
	mesh.indices[31] = 22;
	mesh.indices[32] = 21;
	mesh.indices[33] = 22;
	mesh.indices[34] = 23;
	mesh.indices[35] = 21;
	return mesh;
}

//キューブの生成
void Model::CreateCube() {
	//モデルの読み込み
	modelData_.mesheDatas = { MakeCubeData() };
	//メッシュの再構築
	RebuildMeshes(modelData_.mesheDatas);
	//各種リソースの生成
	CreateResources();
	//テクスチャの適応
	SetTexture(modelData_.mesheDatas[0].materialIndex, "white1x1.png");
	//ノードの初期化
	Node& node = modelData_.rootNode;
	node.name = "cube";
	node.localMatrix = Matrix4x4::Identity4x4();
}

//モデルの生成
void Model::CreateFromModel(const std::string& storedFilePath, const std::string& filename) {
	//モデルの読み込み
	modelData_ = LoadModelFile("engine/resources/models", storedFilePath, filename);
	//メッシュの再構築
	RebuildMeshes(modelData_.mesheDatas);
	//各種リソースの生成
	CreateResources();
}

//モデルの生成(モデルデータ)
void Model::CreateModelFromModelData(const ModelData& modelData) {
	modelData_ = modelData;
	//メッシュの再構成
	RebuildMeshes(modelData_.mesheDatas);
	//各種リソースの生成
	CreateResources();
}

//各種リソースの生成
void Model::CreateResources() {
	//マテリアルリソースの生成
	CreateMaterialResource();
	//リムライトリソースの生成
	CreateRimLightResource();
	//テクスチャの読み込み
	for (MaterialData& materialData : modelData_.material) {
		modelCommon_->GetTextureManager()->LoadTexture(materialData.textureFilePath);
	}
}

//近くの頂点をまとめる
MeshData Model::VertexClusteringByGridSize(const MeshData& meshData, float size) {
	//GridKeyの構造体
	struct GridKey {
		Vector3Int vertexKey;
		Vector2Int texcoordKey;
		Vector3Int normalKey;

		//GridKeyの比較
		bool operator<(const GridKey& g) const {
			if (vertexKey != g.vertexKey) {
				return vertexKey < g.vertexKey;
			} else if (texcoordKey != g.texcoordKey) {
				return texcoordKey < g.texcoordKey;
			}
			return normalKey < g.normalKey;
		}
	};

	//メッシュを取得
	MeshData baseMeshData = meshData;

	//Gridサイズの設定
	float gridSize = size;
	//GridKeyの一覧表
	std::map<GridKey, uint32_t>gridToNewIndices;
	//前のインデックスから新しいインデックスを取得するための対応表
	std::vector<uint32_t>oldToNewIndices(baseMeshData.vertices.size(), UINT32_MAX);
	//新しい頂点
	std::vector<VertexData>newVertices;
	//新しインデックス
	uint32_t newIndex = 0;
	//GridKeyの作成
	for (uint32_t oldIndex = 0; oldIndex < baseMeshData.vertices.size(); oldIndex++) {
		//一つの頂点
		VertexData vertex = baseMeshData.vertices[oldIndex];

		//GridKeyの作成
		GridKey gridKey = {};

		//頂点のキーを作成
		gridKey.vertexKey = {
			static_cast<int32_t>(std::floor(vertex.position.x / gridSize)),
			static_cast<int32_t>(std::floor(vertex.position.y / gridSize)),
			static_cast<int32_t>(std::floor(vertex.position.z / gridSize)),
		};

		//Texcoordのキーを作成
		float uvStep = 0.05f;
		gridKey.texcoordKey = (vertex.texcoord / uvStep).Floor();

		//法線のキーを作成
		float normalStep = 0.2f;
		//一応正規化
		Vector3 normal = vertex.normal.Normalize();
		gridKey.normalKey = ((normal + 1.0f) / normalStep).Floor();

		//gridKeyが一覧表に登録されていたら
		if (gridToNewIndices.contains(gridKey)) {
			//登録済みのインデックスを追加
			oldToNewIndices[oldIndex] = gridToNewIndices[gridKey];
			continue;
		}

		//未登録なら
		//新しい頂点に追加
		newVertices.push_back(vertex);
		//グリッドの一覧表に新しいインデックスを追加
		gridToNewIndices[gridKey] = newIndex;
		//対応表に新しいインデックスを追加
		oldToNewIndices[oldIndex] = newIndex;
		//新しいインデックスの加算
		newIndex++;
	}

	//インデックスの張替え
	std::vector<uint32_t>newIndices;
	for (uint32_t i = 0; i < baseMeshData.indices.size(); i += 3) {
		uint32_t a = oldToNewIndices[baseMeshData.indices[i]];
		uint32_t b = oldToNewIndices[baseMeshData.indices[i + 1]];
		uint32_t c = oldToNewIndices[baseMeshData.indices[i + 2]];

		//三角形が作れない場合は省く
		if (a == b) {
			continue;
		} else if (a == c) {
			continue;
		} else if (b == c) {
			continue;
		}

		//新しいインデックスを追加
		newIndices.push_back(a);
		newIndices.push_back(b);
		newIndices.push_back(c);
	}

	//頂点に代入
	baseMeshData.vertices = newVertices;
	//インデックスに代入
	baseMeshData.indices = newIndices;

	return baseMeshData;
}
