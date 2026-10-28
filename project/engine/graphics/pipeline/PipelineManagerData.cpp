#include "PipelineManagerData.h"
#include "HashUtility.h"

//パイプライン同じか確認
bool PipelineKey::operator==(const PipelineKey& pipelineKey)const{
	return pixelShaderFilePath == pipelineKey.pixelShaderFilePath &&
		pipelineType == pipelineKey.pipelineType &&
		blendMode == pipelineKey.blendMode &&
		cullingMode == pipelineKey.cullingMode &&
		fillMode == pipelineKey.fillMode;
}

//ハッシュキーの作成
size_t PipelineKey::MakeHashKey()const{
	size_t hashValue = 0;
	hashUtility::CreateHash(hashValue, std::hash<std::string>{}(pixelShaderFilePath));
	hashUtility::CreateHash(hashValue, std::hash<PipelineType>{}(pipelineType));
	hashUtility::CreateHash(hashValue, std::hash<BlendMode>{}(blendMode));
	hashUtility::CreateHash(hashValue, std::hash<rasterizerMode::CullingMode>{}(cullingMode));
	hashUtility::CreateHash(hashValue, std::hash<rasterizerMode::FillMode>{}(fillMode));
	return hashValue;
}
