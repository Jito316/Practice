#pragma once
#include "MeshData/MeshData.h"
class Texture;

struct MeshFace
{
	UINT Idx[3];
};

struct Material
{
	std::string				 Name;									// マテリアルの名前

	std::shared_ptr<Texture> spBaseColorTex;						// 基本色のテクスチャ
	Math::Color				 BaseColor = { 1.0f,1.0f, 1.0f, 1.0f };	// 基本色のスケーリング係数（RGBA）

	std::shared_ptr<Texture> spMetallicRoughnessTex;				// B：金属性 G：粗さ
	float					 Metallic = 0.0f;						// 金属性のスケーリング係数
	float					 Roughness = 1.0f;						// 粗さのスケーリング係数

	std::shared_ptr<Texture> spEmissiveTex;							// 自己発光テクスチャ
	Math::Vector3			 Emissive = Math::Vector3::Zero;		// 自己発光のスケーリング係数（RGB）

	std::shared_ptr<Texture> spNormalTex;							// 法線テクスチャ
};

class Mesh
{
public:
	bool Create(GraphicsDevice* _pDevice, const std::vector<MeshVertex>& _vertices, const std::vector<MeshFace>& _face, const Material& _material);

	void DrawInstanced(UINT _vertexCount) const;

	UINT GetInstanceCount()const { return m_instanceCount; };
	const Material& GetMaterial() const { return m_material; }

private:
	Microsoft::WRL::ComPtr<ID3D12Resource>	m_pVBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource>	m_pIBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW				m_vbView = {};
	D3D12_INDEX_BUFFER_VIEW					m_ibView = {};

	UINT m_instanceCount = 0;
	Material								m_material;

	GraphicsDevice* m_pDevice = nullptr;
};