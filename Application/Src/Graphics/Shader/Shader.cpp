#include "Shader.h"

void Shader::Create(GraphicsDevice* _pDevice, const std::wstring& _filePath, const RenderingSetting& _renderingSetting, const std::vector<RangeType>& _rangeTypes)
{
	m_pDevice = _pDevice;

	LoadShaderFile(_filePath);

	m_upRootSignature = std::make_unique<RootSignature>();
	m_upRootSignature->Create(_pDevice, _rangeTypes,m_cbvCount);

	m_upPipeline = std::make_unique<Pipeline>();
	m_upPipeline->SetRenderSettings(_pDevice, m_upRootSignature.get(), _renderingSetting.InputLayout, _renderingSetting.CullMode, _renderingSetting.BlendMode, _renderingSetting.PrimitiveTopologyType);
	m_upPipeline->Create({ m_pVSBlob,m_pHSBlob,m_pDSBlob,m_pGSBlob,m_pPSBlob }, _renderingSetting.Formats, _renderingSetting.IsDepth, _renderingSetting.IsDepthMask, _renderingSetting.RTVCount, _renderingSetting.IsWireFrame);
}

void Shader::Begin(int _w, int _h)
{
	ID3D12GraphicsCommandList& cmdList = *m_pDevice->GetCmdList();

	cmdList.SetPipelineState(m_upPipeline->GetPipeline());
	cmdList.SetGraphicsRootSignature(m_upRootSignature->GetRootSignature());

	D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType = static_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE>(m_upPipeline->GetTopologyType());

	switch (topologyType)
	{
	case D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT:
		cmdList.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		break;
	case D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE:
		cmdList.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		break;
	case D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE:
		cmdList.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	case D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH:
		cmdList.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
		break;
	}

	D3D12_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(_w);
	viewport.Height = static_cast<float>(_h);
	cmdList.RSSetViewports(1, &viewport);

	D3D12_RECT rect = {};
	rect.right = _w;
	rect.bottom = _h;
	cmdList.RSSetScissorRects(1, &rect);
}

void Shader::DrawMesh(const Mesh& _mesh)
{
	_mesh.DrawInstanced();
}

// TODO：シェーダーの使い方法検討中
void Shader::LoadShaderFile(const std::wstring& _filePath)
{
	ID3DInclude* include = D3D_COMPILE_STANDARD_FILE_INCLUDE;
	UINT flag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	ID3DBlob* pErrorBlob = nullptr;

	std::wstring format = L".hlsl";
	std::wstring currentPath = L"Assets/Shader/";

	//VS
	{
		std::wstring fullFilepath = currentPath + _filePath + L"_VS" + format;
		HRESULT hr = D3DCompileFromFile(fullFilepath.data(), nullptr, include, "main", "vs_5_0", flag, 0, &m_pVSBlob, &pErrorBlob);

		if (FAILED(hr))
		{
			assert(false && "頂点シェーダーの作成に失敗しました");
		}
	}

	//HS
	{
		std::wstring fullFilepath = currentPath + _filePath + L"_HS" + format;
		HRESULT hr = D3DCompileFromFile(fullFilepath.data(), nullptr, include, "main", "hs_5_0", flag, 0, &m_pHSBlob, &pErrorBlob);
	}

	//DS
	{
		std::wstring fullFilepath = currentPath + _filePath + L"_DS" + format;
		HRESULT hr = D3DCompileFromFile(fullFilepath.data(), nullptr, include, "main", "ds_5_0", flag, 0, &m_pDSBlob, &pErrorBlob);
	}

	//GS
	{
		std::wstring fullFilepath = currentPath + _filePath + L"_GS" + format;
		HRESULT hr = D3DCompileFromFile(fullFilepath.data(), nullptr, include, "main", "gs_5_0", flag, 0, &m_pGSBlob, &pErrorBlob);
	}

	//VS
	{
		std::wstring fullFilepath = currentPath + _filePath + L"_PS" + format;
		HRESULT hr = D3DCompileFromFile(fullFilepath.data(), nullptr, include, "main", "ps_5_0", flag, 0, &m_pPSBlob, &pErrorBlob);

		if (FAILED(hr))
		{
			assert(false && "ピクセルシェーダーの作成に失敗しました");
		}
	}
}
