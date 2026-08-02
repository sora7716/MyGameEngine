#include "RendererData.h"

//サイズがあっている確認
bool LODRenderData::IsLodCountValid() const{
	return modelRendererDatas.size() == srvIndices.size()
		&& srvIndices.size() == drawCounts.size();
}
