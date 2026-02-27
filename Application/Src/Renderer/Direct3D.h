#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

class WindowsWindow;

class Direct3D
{
public:
	bool Initialize(WindowsWindow* _window);
	void Finalize();

	void Render();
private:
	Microsoft::WRL::ComPtr<ID3D12Device> m_device;
	Microsoft::WRL::ComPtr<IDXGIFactory6> m_dxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_cmdAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_cmdList;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_cmdQueue;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeaps;

	Microsoft::WRL::ComPtr<ID3D12Fence> m_pFence;
	UINT64 m_fenceVal = 0;
	HANDLE m_fenveEvent = nullptr;

	std::vector<ID3D12Resource*> m_backBuffers;

	bool CreateDevice();
	bool CreateCommandObjects();
	bool CreateSwapChain(WindowsWindow* _window);
	bool CreateBuffer();

	IDXGIAdapter* FindAdapter();

	Direct3D() = default;
	Direct3D(Direct3D&&) = default;
	Direct3D(Direct3D&) = default;
public:
	static Direct3D& Instance()
	{
		static Direct3D instance;
		return instance;
	}
};