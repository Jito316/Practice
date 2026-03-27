#pragma once

enum class HeapType
{
	CBVSRVUAV = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
	SAMPLER = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
	RTV = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
	DSV = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
};

template<typename T>
class Heap
{
public:
	Heap() = default;
	virtual ~Heap() = default;

	bool Create(GraphicsDevice* _pDevice, HeapType _heapType,T _useCount)
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Type = static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(_heapType);
		heapDesc.NodeMask = 0;
		heapDesc.NumDescriptors = ComputUseCount(_useCount);
		heapDesc.Flags = _heapType == HeapType::CBVSRVUAV ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		HRESULT hr = _pDevice->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_pHeap));
		if (FAILED(hr))
		{
			assert(false && "ヒープ作成失敗");
			return false;
		}

		m_useCount = _useCount;
		m_incrementSize = _pDevice->GetDevice()->GetDescriptorHandleIncrementSize(static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(_heapType));
		m_pDevice = _pDevice;

		return true;
	}

	virtual const D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(int _num)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pHeap->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += (UINT64)m_incrementSize * _num;
		return handle;
	}
	
	virtual const D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(int _num)
	{
		D3D12_GPU_DESCRIPTOR_HANDLE handle = m_pHeap->GetGPUDescriptorHandleForHeapStart();
		handle.ptr += (UINT64)m_incrementSize * _num;
		return handle;
	}

protected:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pHeap;

	T m_useCount = T();
	int m_incrementSize = 0;
	int m_nextRegistNumber = 0;

	GraphicsDevice* m_pDevice = nullptr;

private:
	UINT ComputUseCount(UINT _useCount)
	{
		return _useCount;
	}

	UINT ComputUseCount(const Math::Vector3& _useCount)
	{
		return (UINT)(_useCount.x + _useCount.y + _useCount.z);
	}
};