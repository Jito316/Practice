#include "Mesh.h"
#include "../Direct3D.h"

bool Mesh::Create(Direct3D* _pDevice)
{
	m_vertices[0] = { -0.5f,-0.7f, 0.0f };
	m_vertices[1] = { 0.0f,0.7f, 0.0f };
	m_vertices[2] = { 0.5f,-0.7f, 0.0f };

	D3D12_HEAP_PROPERTIES heapprop = {};
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeof(m_vertices);
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	HRESULT hr = _pDevice->GetDevice()->CreateCommittedResource(
		&heapprop, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ
		, nullptr, IID_PPV_ARGS(&m_pVBuffer));

	if (FAILED(hr))
	{
		assert(false && "頂点バッファの作成失敗");
		return false;
	}

	m_vbView.BufferLocation = m_pVBuffer->GetGPUVirtualAddress();
	m_vbView.SizeInBytes = sizeof(m_vertices);
	m_vbView.StrideInBytes = sizeof(m_vertices[0]);
	_pDevice->GetCmdList()->IASetVertexBuffers(0, 1, &m_vbView);

	Math::Vector3* vertexMap = nullptr;
	{
		hr = m_pVBuffer->Map(0, nullptr, (void**)&vertexMap);
		std::copy(std::begin(m_vertices), std::end(m_vertices), vertexMap);
		m_pVBuffer->Unmap(0, nullptr);
	}

	m_pDevice = _pDevice;

	return true;
}

void Mesh::DrawInstanced()const
{
	ID3D12GraphicsCommandList& cmdList = *m_pDevice->GetCmdList();
	cmdList.IASetVertexBuffers(0, 1, &m_vbView);
	cmdList.DrawInstanced(3, 1, 0, 0);
}
