#include "CBufferAllocator.h"
#include "../Heap/CBVSRVUAVHeap/CBVSRVUAVHeap.h"

void CBufferAllocator::Create(GraphicsDevice* _pDevice, CBVSRVUAVHeap* _pHeap)
{
	m_pDevice = _pDevice;
	m_pHeap = _pHeap;

	D3D12_HEAP_PROPERTIES heapprop = {};
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Width = (UINT64)((1 + 0xff) & ~0xff) * (int)m_pHeap->GetUseCount().x;

	HRESULT hr = m_pDevice->GetDevice()->CreateCommittedResource(&heapprop, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pBuffer));
	if (FAILED(hr)) 
	{
		assert(false && "CBufferAllocatorの作成失敗");
		return;
	}

	m_pBuffer->Map(0, nullptr, (void**)&m_pMappedBuffer);
}

void CBufferAllocator::ResetCurrentUseNumber()
{
	m_currentUseNumber = 0;
}

void CBufferAllocator::BindAndAttachDataInternal(int _descIndex,const void* _pData, int _size)
{
	int sizeAligned = (_size * 0xff) & ~0xff;
	int useValue = sizeAligned / 0x100;

	if (m_currentUseNumber + useValue > (int)m_pHeap->GetUseCount().x)
	{
		assert(false && "使用できるヒープ容量を超えました。");
		return;
	}

	int top = m_currentUseNumber;
	memcpy(m_pMappedBuffer + top, _pData, _size);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbDesc = {};
	cbDesc.BufferLocation = m_pBuffer->GetGPUVirtualAddress() + (UINT)top * 0x100;
	cbDesc.SizeInBytes = sizeAligned;

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_pHeap->GetHeap()->GetCPUDescriptorHandleForHeapStart();
	cpuHandle.ptr += (UINT64)m_pDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * m_currentUseNumber;

	m_pDevice->GetDevice()->CreateConstantBufferView(&cbDesc, cpuHandle);

	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = m_pHeap->GetHeap()->GetGPUDescriptorHandleForHeapStart();
	gpuHandle.ptr += (UINT64)m_pDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * m_currentUseNumber;

	m_pDevice->GetCmdList()->SetGraphicsRootDescriptorTable(_descIndex, gpuHandle);

	m_currentUseNumber += useValue;
}
