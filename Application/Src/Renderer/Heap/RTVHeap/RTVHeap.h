#pragma once
#include "../Heap.h"

class RTVHeap :public Heap<HeapType::RTV>
{
public:
	int CreateRTV(ID3D12Resource* pBuffer);

};