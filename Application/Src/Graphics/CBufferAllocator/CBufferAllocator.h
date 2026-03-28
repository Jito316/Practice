#pragma once
class CBVSRVUAVHeap;

class CBufferAllocator
{
public:
	void Create(GraphicsDevice* _pDevice, CBVSRVUAVHeap* _pHeap);

	void ResetCurrentUseNumber();

	template<typename T>
	void BindAndAttachData(int _descIndex, const T& _data);

private:
	void BindAndAttachDataInternal(int _descIndex,const void* _pData, int _size);

	Microsoft::WRL::ComPtr<ID3D12Resource> m_pBuffer = nullptr;
	struct { char buf[256]; }*m_pMappedBuffer = nullptr;
	int m_currentUseNumber = 0;

	GraphicsDevice* m_pDevice = nullptr;
	CBVSRVUAVHeap* m_pHeap = nullptr;
};

template<typename T>
inline void CBufferAllocator::BindAndAttachData(int _descIndex, const T& _data)
{
	if (!m_pHeap)return;
	BindAndAttachDataInternal(_descIndex, &_data, sizeof(T));
}
