#pragma once
#include "../Mesh/Mesh.h"
#include "../Model/Model.h"
#include "Pipeline/Pipeline.h"
#include "RootSignature/RootSignature.h"

struct RenderingSetting 
{
	std::vector<InputLayout> InputLayout;
	std::vector<DXGI_FORMAT> Formats;
	CullMode CullMode = CullMode::Back;
	BlendMode BlendMode = BlendMode::Appha;
	PrimitiveTopologyType PrimitiveTopologyType = PrimitiveTopologyType::Triangle;
	bool IsDepth = true;
	bool IsDepthMask = true;
	int RTVCount = 1;
	bool IsWireFrame = false;
};

class Shader 
{
public:
	void Create(GraphicsDevice* _pDevice, const std::wstring& _filePath, const RenderingSetting& _renderingSetting, const std::vector<RangeType>& _rangeTypes);

	void Begin(int _w, int _h);
	void DrawMesh(const Mesh& _mesh);
	void DrawModel(const ModelData& _model);

	UINT GetCBVCount() const { return m_cbvCount; }

private:
	void LoadShaderFile(const std::wstring& _filePath);
	void SetMaterial(const Material& _material)const;

	std::unique_ptr<Pipeline> m_upPipeline = nullptr;
	std::unique_ptr<RootSignature> m_upRootSignature = nullptr;

	ID3DBlob* m_pVSBlob = nullptr;
	ID3DBlob* m_pHSBlob = nullptr;
	ID3DBlob* m_pDSBlob = nullptr;
	ID3DBlob* m_pGSBlob = nullptr;
	ID3DBlob* m_pPSBlob = nullptr;

	UINT m_cbvCount = 0;

	GraphicsDevice* m_pDevice = nullptr;
};