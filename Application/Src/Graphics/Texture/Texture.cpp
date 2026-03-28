#include "Texture.h"
#include "../Heap/CBVSRVUAVHeap/CBVSRVUAVHeap.h"

bool Texture::Load(GraphicsDevice* _pDevice, const std::string& _filepath)
{
	wchar_t wFilepath[128];
	MultiByteToWideChar(CP_ACP, 0, _filepath.c_str(), -1, wFilepath, _countof(wFilepath));

	DirectX::TexMetadata metadata = {};
	DirectX::ScratchImage scratchImage = {};
	const DirectX::Image* pImage = nullptr;

	HRESULT hr = DirectX::LoadFromWICFile(wFilepath, DirectX::WIC_FLAGS_NONE, &metadata, scratchImage);
	if (FAILED(hr))
	{
		assert(false && "テクスチャ読み込み失敗");
		return false;
	}

	pImage = scratchImage.GetImage(0, 0, 0);

	D3D12_HEAP_PROPERTIES heapprop = {};
	heapprop.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
	resourceDesc.Format = metadata.format;
	resourceDesc.Width = (UINT16)metadata.width;
	resourceDesc.Height = (UINT16)metadata.height;
	resourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	resourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	resourceDesc.SampleDesc.Count = 1;

	hr = _pDevice->GetDevice()->CreateCommittedResource(&heapprop, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pBuffer));
	if (FAILED(hr))
	{
		assert(false && "テクスチャバッファ作成失敗");
		return false;
	}

	hr = m_pBuffer->WriteToSubresource(0, nullptr, pImage->pixels, (UINT)pImage->rowPitch, (UINT)pImage->slicePitch);
	if (FAILED(hr))
	{
		assert(false && "バッファにテクスチャデータの書き込み失敗");
		return false;
	}

	m_srvNumber = _pDevice->GetCBVSRVUAVHeap()->CreateSRV(m_pBuffer.Get());
	m_pDevice = _pDevice;

	return true;
}

void Texture::Set(int _index)
{
	m_pDevice->GetCmdList()->SetGraphicsRootDescriptorTable
	(_index, m_pDevice->GetCBVSRVUAVHeap()->GetGPUHandle(m_srvNumber));
}