#pragma once

enum class HeapType
{
	CBVSRVUAV = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
	SAMPLER = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
	RTV = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
	DSV = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
};

template<HeapType type = HeapType::RTV>
class Heap
{
public:
	Heap() = default;
	virtual ~Heap() = default;

	bool Create(ID3D12Device* _pDevice, int _useCount)
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Type = static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(type);
		heapDesc.NodeMask = 0;
		heapDesc.NumDescriptors = _useCount;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		auto hr = _pDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_pHeap));
		if (FAILED(hr))
		{
			assert(false && "ヒープ作成失敗");
			return false;
		}

		m_useCount = _useCount;
		m_incrementSize = _pDevice->GetDescriptorHandleIncrementSize(static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(type));
		m_pDevice = _pDevice;

		return true;

	}

	virtual D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(int _num)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pHeap->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += (UINT64)m_incrementSize * _num;
		return handle;
	}
	
	virtual D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(int _num)
	{
		D3D12_GPU_DESCRIPTOR_HANDLE handle = m_pHeap->GetGPUDescriptorHandleForHeapStart();
		handle.ptr += (UINT64)m_incrementSize * _num;
		return handle;
	}

protected:
	ID3D12Device* m_pDevice = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pHeap;

	int m_useCount = 0;
	int m_incrementSize = 0;
	int m_nextRegistNumber = 0;

	UINT ComputUseCount(UINT _useCount)
	{
		return _useCount;
	}

	UINT ComputUseCount(const Math::Vector3& _useCount)
	{
		return (UINT)(_useCount.x + _useCount.y + _useCount.z);
	}
};