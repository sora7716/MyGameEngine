#define NOMINMAX
#include "LODBuilder.h"
#include "Logger.h"
#include "Model.h"
#include "HashUtility.h"
#include <array>
#include <queue>
#include <unordered_map>
#include <algorithm>

//uint32_tの変数を二つ切り詰めてuint64_tの検索キーを作成
static uint64_t MakeEdgeKey(uint32_t a, uint32_t b) {
	std::array<uint32_t, 2>edge = {};
	edge = { std::min(a,b),std::max(a,b) };
	return static_cast<uint64_t>(edge[0]) << 32 | static_cast<uint64_t>(edge[1]);
}

//コンストラクタ
LODBuilder::LODBuilder() {
}

//デストラクタ
LODBuilder::~LODBuilder() {
}

//LODモデルの生成
void LODBuilder::CreateLODModel(Model* model, const std::vector<float>& keepRates) {
	//頂点合成する割合が存在しなかったら
	if (keepRates.empty()) {
		return;
	}

	//サイズを決定
	lodModels_.resize(keepRates.size());
	//モデルの作成
	for (uint32_t i = 0; i < keepRates.size(); i++) {
		lodModels_[i] = Model::CreateModel(model->GetModelCommon(), model->GetModelData());
		//lodModels_[i]->RebuildMeshes(VertexClustering(lodModels_[i]->GetModelData().meshDatas, keepRates[i]));
		lodModels_[i]->RebuildMeshes(EdgeCollapse(lodModels_[i]->GetModelData().meshDatas, keepRates[i]));
	}
}

//LODモデルの取得
Model* LODBuilder::GetLODModel(uint32_t lodIndex) {
	return lodModels_[lodIndex].get();
}

//カラーの設定
void LODBuilder::SetColor(uint32_t materialIndex, const Vector4& color) {
	for (std::unique_ptr<Model>& lodModel : lodModels_) {
		if (lodModel) {
			lodModel->SetColor(materialIndex, color);
		}
	}
}

//テクスチャの設定
void LODBuilder::SetTexture(uint32_t materialIndex, const std::string& imageFileName) {
	for (std::unique_ptr<Model>& lodModel : lodModels_) {
		if (lodModel) {
			lodModel->SetTexture(materialIndex, imageFileName);
		}
	}
}

//環境マップの映り込み度を調整
void LODBuilder::SetEnvironmentCoefficient(uint32_t materialIndex, float environmentCoefficient) {
	for (std::unique_ptr<Model>& lodModel : lodModels_) {
		if (lodModel) {
			lodModel->SetEnvironmentCoefficient(materialIndex, environmentCoefficient);
		}
	}
}

//環境マップの設定
void LODBuilder::SetEnvironmentMap(uint32_t materialIndex, const std::string& environmentMapFileName) {
	for (std::unique_ptr<Model>& lodModel : lodModels_) {
		if (lodModel) {
			lodModel->SetEnvironmentMap(materialIndex, environmentMapFileName);
		}
	}
}

//ライティングフラグの設定
void LODBuilder::SetIsLighting(uint32_t materialIndex, bool isLighting) {
	for (std::unique_ptr<Model>& lodModel : lodModels_) {
		if (lodModel) {
			lodModel->SetIsLighting(materialIndex, isLighting);
		}
	}
}

//輝度の設定
void LODBuilder::SetShininess(uint32_t materialIndex, float shininess) {
	for (std::unique_ptr<Model>& lodModel : lodModels_) {
		if (lodModel) {
			lodModel->SetShininess(materialIndex, shininess);
		}
	}
}

//モデルのサイズを取得
uint32_t LODBuilder::LODModelSize()const {
	return static_cast<uint32_t>(lodModels_.size());
}

//グリッドサイズの最小値の設定
void LODBuilder::SetMinGridSize(float minGridSize) {
	minGridSize_ = minGridSize;
}

//グリッドサイズの最大値の設定
void LODBuilder::SetMaxGridSize(float maxGridSize) {
	maxGridSize_ = maxGridSize;
}

//辺縮約
std::vector<MeshData> LODBuilder::EdgeCollapse(const std::vector<MeshData>& meshData, float rate) {
	//メッシュデータを記録
	std::vector<MeshData> baseMeshDatas = meshData;
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
			uint32_t batchCount = 50000;
			float batchRate = 50.0f;
			//割合ごとにCollapseしたい数と割合も変更
			if (rate <= 0.25f) {
				batchCount = 700000;
				batchRate = 200.0f;
			} else if (rate <= 0.5f) {
				batchCount = 500000;
				batchRate = 100.f;
			} else if (rate <= 0.8f) {
				batchCount = 300000;
				batchRate = 80.0f;
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

//近くの頂点をまとめる
MeshData LODBuilder::VertexClusteringByGridSize(const MeshData& meshData, float size) {
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

		//キーがすべて同じかを判定
		bool operator==(const GridKey& other)const {
			//頂点のキーが一致してるか
			bool isMatchVertexKey =
				vertexKey.x == other.vertexKey.x &&
				vertexKey.y == other.vertexKey.y &&
				vertexKey.z == other.vertexKey.z;

			//Texcoordのキーが一致してるか
			bool isMatchTexcoordKey =
				texcoordKey.x == other.texcoordKey.x &&
				texcoordKey.y == other.texcoordKey.y;

			//法線のキーが一致してるか
			bool isMatchNormalKey =
				normalKey.x == other.normalKey.x &&
				normalKey.y == other.normalKey.y &&
				normalKey.z == other.normalKey.z;

			return isMatchVertexKey && isMatchTexcoordKey && isMatchNormalKey;
		}
	};

	//グリッドキーごとの情報をまとめた
	struct GridCluster {
		Vector4 vertexPosSum = {};//GridKeyに入った頂点の位置の合計
		uint32_t vertexCount = 0;//GridKeyに入った頂点数
		uint32_t newIndex = 0;//代表頂点のインデックス
	};

	//グリッドキーのハッシュ
	struct GridKeyHash {
		size_t operator()(const GridKey& key)const {
			size_t seed = 0;

			hashUtility::CreateHash(seed, key.vertexKey.x);
			hashUtility::CreateHash(seed, key.vertexKey.y);
			hashUtility::CreateHash(seed, key.vertexKey.z);

			hashUtility::CreateHash(seed, key.texcoordKey.x);
			hashUtility::CreateHash(seed, key.texcoordKey.y);

			hashUtility::CreateHash(seed, key.normalKey.x);
			hashUtility::CreateHash(seed, key.normalKey.y);
			hashUtility::CreateHash(seed, key.normalKey.z);

			return seed;
		}
	};

	//メッシュを取得
	MeshData baseMeshData = meshData;

	//Gridサイズの設定
	float gridSize = size;
	//Gridごとの処理
	std::unordered_map<GridKey, GridCluster, GridKeyHash>gridClusters;
	gridClusters.reserve(baseMeshData.vertices.size());
	//前のインデックスから新しいインデックスを取得するための対応表
	std::vector<uint32_t>oldToNewIndices(baseMeshData.vertices.size(), UINT32_MAX);
	//新しい頂点
	std::vector<VertexData>newVertices;
	newVertices.reserve(baseMeshData.vertices.size());
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

		//gridKeyがgridClustersに登録されてるか
		auto [it, inserted] = gridClusters.try_emplace(gridKey);
		GridCluster& gridCluster = it->second;
		if (inserted) {
			//未登録
			gridCluster.vertexPosSum = vertex.position;
			gridCluster.vertexCount = 1;
			gridCluster.newIndex = newIndex;

			//新しい頂点データを作成
			VertexData newVertex = {
				.position = vertex.position,
				.texcoord = vertex.texcoord,
				.normal = vertex.normal
			};

			//新しい頂点を追加
			newVertices.push_back(newVertex);
			//対応表に新しいインデックスを追加
			oldToNewIndices[oldIndex] = newIndex;
			//新しいインデックスの加算
			newIndex++;

		} else {
			//登録済み

			//頂点を加算
			gridCluster.vertexPosSum += vertex.position;
			//頂点数の数を加算
			gridCluster.vertexCount++;

			//頂点の平均値を求める
			Vector4 averagePos = {
				gridCluster.vertexPosSum.x / static_cast<float>(gridCluster.vertexCount),
				gridCluster.vertexPosSum.y / static_cast<float>(gridCluster.vertexCount),
				gridCluster.vertexPosSum.z / static_cast<float>(gridCluster.vertexCount),
				1.0f
			};

			//頂点の更新
			newVertices[gridCluster.newIndex].position = averagePos;
			//頂点の対応表に登録	
			oldToNewIndices[oldIndex] = gridCluster.newIndex;
		}
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

//頂点を合成する
std::vector<MeshData> LODBuilder::VertexClustering(const std::vector<MeshData>& meshData, float rate) {
	//メッシュ
	std::vector<MeshData>baseMeshes = meshData;

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
	for (const MeshData& mesh : baseMeshes) {
		uint32_t goalVertexCount = static_cast<uint32_t>(static_cast<float>(mesh.vertices.size()) * rate);
		//グリッドサイズ
		float minGridSize = minGridSize_;//最小値
		float maxGridSize = maxGridSize_;//最大値

		//目標の頂点数に一番違いメッシュ
		MeshData bestMesh = mesh;
		uint32_t bestMeshVertexCount = static_cast<uint32_t>(bestMesh.vertices.size());
		float gridSize = 0;
		//試行回数
		const uint32_t kTrialCount = 20;
		//二分探索
		for (uint32_t i = 0; i < kTrialCount; i++) {
			//gridSizeはminとmaxの中間
			gridSize = (minGridSize + maxGridSize) / 2.0f;

			//元メッシュをgridSizeでクラスタリング
			MeshData trialMesh = VertexClusteringByGridSize(mesh, gridSize);
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
