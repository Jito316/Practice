#include "Mesh.h"

bool Mesh::Create(GraphicsDevice* _pDevice)
{
	m_pDevice = _pDevice;
	auto& device = *m_pDevice->GetDevice();
	auto& cmdList = *m_pDevice->GetCmdList();

	//頂点情報
	m_vertices.emplace_back(Math::Vector3{ -0.5f,-0.5f, 0.0f }, Math::Vector2{ 0.0f,1.0f });
	m_vertices.emplace_back(Math::Vector3{ -0.5f,0.5f, 0.0f }, Math::Vector2{ 0.0f,0.0f });
	m_vertices.emplace_back(Math::Vector3{ 0.5f,-0.5f, 0.0f }, Math::Vector2{ 1.0f,1.0f });
	m_vertices.emplace_back(Math::Vector3{ 0.5f,0.5f, 0.0f }, Math::Vector2{ 1.0f,0.0f });

	//頂点バッファの作成
	D3D12_HEAP_PROPERTIES heapprop = {};
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeof(Vertex) * m_vertices.size();
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	HRESULT hr = device.CreateCommittedResource(
		&heapprop, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ
		, nullptr, IID_PPV_ARGS(&m_pVBuffer));

	if (FAILED(hr))
	{
		assert(false && "頂点バッファの作成失敗");
		return false;
	}

	//頂点バッファビューの設定
	m_vbView.BufferLocation = m_pVBuffer->GetGPUVirtualAddress();
	m_vbView.SizeInBytes = (UINT)resourceDesc.Width;
	m_vbView.StrideInBytes = sizeof(Vertex);
	cmdList.IASetVertexBuffers(0, 1, &m_vbView);

	Vertex* vbMap = nullptr;
	{
		hr = m_pVBuffer->Map(0, nullptr, (void**)&vbMap);
		std::copy(std::begin(m_vertices), std::end(m_vertices), vbMap);
		m_pVBuffer->Unmap(0, nullptr);
	}


	//インデックス情報
	m_indeices.emplace_back(0);
	m_indeices.emplace_back(1);
	m_indeices.emplace_back(2);
	m_indeices.emplace_back(2);
	m_indeices.emplace_back(1);
	m_indeices.emplace_back(3);

	//インデックスバッファの作成
	resourceDesc.Width = sizeof(UINT) * m_indeices.size();

	hr = device.CreateCommittedResource(
		&heapprop, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ
		, nullptr, IID_PPV_ARGS(&m_pIBuffer));

	if (FAILED(hr))
	{
		assert(false && "インデックスバッファーの作成失敗");
		return false;
	}

	//インデックスビューの設定
	m_ibView.BufferLocation = m_pIBuffer->GetGPUVirtualAddress();
	m_ibView.SizeInBytes = (UINT)resourceDesc.Width;
	m_ibView.Format = DXGI_FORMAT_R32_UINT;
	cmdList.IASetIndexBuffer(&m_ibView);

	UINT* ibMap = nullptr;
	{
		hr = m_pIBuffer->Map(0, nullptr, (void**)&ibMap);
		std::copy(std::begin(m_indeices), std::end(m_indeices), ibMap);
		m_pIBuffer->Unmap(0, nullptr);
	}


	return true;
}

void Mesh::DrawInstanced()const
{
	ID3D12GraphicsCommandList& cmdList = *m_pDevice->GetCmdList();
	cmdList.IASetVertexBuffers(0, 1, &m_vbView);
	cmdList.IASetIndexBuffer(&m_ibView);
	cmdList.DrawIndexedInstanced(6, 1, 0, 0, 0);
}
