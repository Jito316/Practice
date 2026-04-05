#pragma once

#ifdef _DEBUG
#pragma comment(lib,"assimp-vc143-mtd.lib")
#else
#pragma comment(lib,"assimp-vc143-mt.lib")
#endif // _DEBUG

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Model.h"
#include "../Mesh/Mesh.h"


class ModelLoader 
{
public:
	bool Load(const std::string& _filepath,std::vector<ModelData::Node>& _nodes);

private:
	std::shared_ptr<Mesh> Parse(const aiScene* _pScene, const aiMesh* _pMesh,const aiMaterial* _pMaterial,const std::string& _dirpath);
	const Material ParseMaterial(const aiMaterial* _pMaterial, const std::string& _dirpath);

};