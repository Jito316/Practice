#include "ModelLoader.h"
#include "../Texture/Texture.h"

bool ModelLoader::Load(const std::string& _filepath, std::vector<ModelData::Node>& _nodes)
{
	Assimp::Importer importer;

	int flag = 0;
	flag |= aiProcess_CalcTangentSpace;
	flag |= aiProcess_Triangulate;
	flag |= aiProcess_GenSmoothNormals;
	flag |= aiProcess_PreTransformVertices;
	flag |= aiProcess_RemoveRedundantMaterials;
	flag |= aiProcess_GenUVCoords;
	flag |= aiProcess_OptimizeMeshes;

	const aiScene* pScene = importer.ReadFile(_filepath, flag);
	if (pScene == nullptr)
	{
		assert(false && "モデルのファイルが見つかり居ません");
		return false;
	}

	_nodes.resize(pScene->mNumMeshes);

	std::string dirpath = GetDirFromPath(_filepath);

	for (UINT i = 0; i < pScene->mNumMeshes; ++i)
	{
		aiMesh* pMesh = pScene->mMeshes[i];
		aiMaterial* pMaterial = pScene->mMaterials[i];
		_nodes[i].spMesh = Parse(pScene, pMesh, pMaterial, dirpath);
	}

	return true;
}

std::shared_ptr<Mesh> ModelLoader::Parse(const aiScene* _pScene, const aiMesh* _pMesh, const aiMaterial* _pMaterial, const std::string& _dirpath)
{
	std::vector<MeshVertex> vertices;
	std::vector<MeshFace> faces;

	vertices.resize(_pMesh->mNumVertices);
	for (UINT i = 0; i < _pMesh->mNumVertices; ++i)
	{
		vertices[i].Position.x = _pMesh->mVertices[i].x;
		vertices[i].Position.y = _pMesh->mVertices[i].y;
		vertices[i].Position.z = _pMesh->mVertices[i].z;

		if (_pMesh->HasTextureCoords(0))
		{
			vertices[i].UV.x = static_cast<float>(_pMesh->mTextureCoords[0][i].x);
			vertices[i].UV.y = static_cast<float>(_pMesh->mTextureCoords[0][i].y);
		}

		vertices[i].Normal.x = _pMesh->mNormals[i].x;
		vertices[i].Normal.y = _pMesh->mNormals[i].y;
		vertices[i].Normal.z = _pMesh->mNormals[i].z;

		if (_pMesh->HasTangentsAndBitangents())
		{
			vertices[i].Tangent.x = _pMesh->mTangents[i].x;
			vertices[i].Tangent.y = _pMesh->mTangents[i].y;
			vertices[i].Tangent.z = _pMesh->mTangents[i].z;
		}

		if (_pMesh->HasVertexColors(0))
		{
			Math::Color color;
			color.x = _pMesh->mColors[0][i].r;
			color.y = _pMesh->mColors[0][i].g;
			color.z = _pMesh->mColors[0][i].b;

			vertices[i].Color = color.RGBA().v;
		}
	}


	faces.resize(_pMesh->mNumFaces);

	for (UINT i = 0; i < _pMesh->mNumFaces; ++i)
	{
		faces[i].Idx[0] = _pMesh->mFaces[i].mIndices[0];
		faces[i].Idx[1] = _pMesh->mFaces[i].mIndices[1];
		faces[i].Idx[2] = _pMesh->mFaces[i].mIndices[2];
	}

	std::shared_ptr<Mesh>spMesh = std::make_shared<Mesh>();
	spMesh->Create(&GraphicsDevice::Instance(), vertices, faces, ParseMaterial(_pMaterial, _dirpath));

	return spMesh;
}

const Material ModelLoader::ParseMaterial(const aiMaterial* _pMaterial, const std::string& _dirpath)
{
	Material material = {};

	{
		aiString name;
		if (_pMaterial->Get(AI_MATKEY_NAME, name) == AI_SUCCESS)
		{
			material.Name = name.C_Str();
		}
	}

	{
		aiString path;
		if (_pMaterial->GetTexture(AI_MATKEY_BASE_COLOR_TEXTURE, &path) == AI_SUCCESS)
		{
			std::string filepath = std::string(path.C_Str());

			material.spBaseColorTex = std::make_shared<Texture>();
			if (material.spBaseColorTex->Load(&GraphicsDevice::Instance(), _dirpath + filepath) == false)
			{
				assert(0 && "Diffuseテクスチャのロードに失敗");
				return Material();
			}
		}
	}

	{
		aiColor4D diffuse;
		if (_pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) == AI_SUCCESS)
		{
			material.BaseColor.x = diffuse.r;
			material.BaseColor.y = diffuse.g;
			material.BaseColor.z = diffuse.b;
			material.BaseColor.w = diffuse.a;
		}

	}


	{
		aiString path;
		if (_pMaterial->GetTexture(AI_MATKEY_METALLIC_TEXTURE, &path) == AI_SUCCESS||
		_pMaterial->GetTexture(AI_MATKEY_ROUGHNESS_TEXTURE, &path) == AI_SUCCESS) 
		{
			std::string filepath = std::string(path.C_Str());

			material.spMetallicRoughnessTex = std::make_shared<Texture>();
			if (material.spMetallicRoughnessTex->Load(&GraphicsDevice::Instance(), _dirpath + filepath) == false)
			{
				assert(0 && "MetallicRoughnessテクスチャのロードに失敗");
				return Material();
			}
		}
	}

	{
		float metallic = 0.0f;
		if (_pMaterial->Get(AI_MATKEY_METALLIC_FACTOR,metallic) == AI_SUCCESS)
		{
			material.Metallic = metallic;
		}
	}

	{
		float roughness = 0.0f;
		if (_pMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) == AI_SUCCESS)
		{
			material.Roughness = roughness;
		}
	}

	{
		aiString path;
		if (_pMaterial->GetTexture(AI_MATKEY_EMISSIVE_TEXTURE, &path) == AI_SUCCESS)
		{
			std::string filepath = std::string(path.C_Str());

			material.spEmissiveTex = std::make_shared<Texture>();
			if (material.spEmissiveTex->Load(&GraphicsDevice::Instance(), _dirpath + filepath) == false)
			{
				assert(0 && "Emissiveテクスチャのロードに失敗");
				return Material();
			}
		}
	}

	{
		aiColor3D emissive;
		if (_pMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissive) == AI_SUCCESS)
		{
			material.Emissive.x = emissive.r;
			material.Emissive.y = emissive.g;
			material.Emissive.z = emissive.b;
		}
	}

	{
		aiString path;
		if (_pMaterial->GetTexture(AI_MATKEY_NORMAL_TEXTURE, &path) == AI_SUCCESS)
		{
			std::string filepath = std::string(path.C_Str());

			material.spNormalTex = std::make_shared<Texture>();
			if (material.spNormalTex->Load(&GraphicsDevice::Instance(), _dirpath + filepath) == false)
			{
				assert(0 && "Normalテクスチャのロードに失敗");
				return Material();
			}
		}
	}
	
	return material;
}
