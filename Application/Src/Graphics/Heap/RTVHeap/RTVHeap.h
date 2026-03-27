#pragma once
#include "../Heap.h"

class RTVHeap :public Heap<int>
{
public:
	int CreateRTV(ID3D12Resource* pBuffer);

};