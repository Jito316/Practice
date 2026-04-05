#include "Model.h"
#include "ModelLoader.h"

bool ModelData::Load(const std::string& _filepath)
{
	ModelLoader modelLoader;

	if (modelLoader.Load(_filepath, m_nodes) == false)
	{
		assert(false && "モデルのロードに失敗");
		return false;
	}

	return true;
}
