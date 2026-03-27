#pragma once

class WindowsWindow;
class RTVHeap;
class CBVSRVUAVHeap;
class Shader;

class GraphicsDevice
{
public:
	bool Initialize(WindowsWindow* _window);
	void Finalize();

	void Prepare();
	void ScreenFlip();

	ID3D12Device* GetDevice() const { return m_device.Get(); }
	ID3D12GraphicsCommandList* GetCmdList() const { return m_cmdList.Get(); }

	CBVSRVUAVHeap* GetCBVSRVUAVHeap() const{ return m_spCBVSRVUAVHeap.get(); }

private:
	void SetResourceBarrier(ID3D12Resource* _pResource,D3D12_RESOURCE_STATES _stateBefore, D3D12_RESOURCE_STATES _stateAfter);

	enum class GPUTier
	{
		NVIDIA,
		Amd,
		Intel,
		Arm,
		Qualcomm,
		Kind,
	};

	Microsoft::WRL::ComPtr<ID3D12Device> m_device;
	Microsoft::WRL::ComPtr<IDXGIFactory6> m_dxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_cmdAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_cmdList;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_cmdQueue;


	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_backBuffers;
	std::shared_ptr<RTVHeap> m_spRTVHeap;
	std::shared_ptr<CBVSRVUAVHeap> m_spCBVSRVUAVHeap;

	Microsoft::WRL::ComPtr<ID3D12Fence> m_pFence;

	Microsoft::WRL::ComPtr<IDXGIAdapter> m_pAdapter;

	UINT64 m_fenceVal = 0;

	bool CreateFactory();
	bool CreateDevice();
	bool CreateCommandObjects();
	bool CreateSwapChain(WindowsWindow* _window);
	bool CreateRTVBuffer();
	bool CreateFence();

	Microsoft::WRL::ComPtr<IDXGIAdapter> FindAdapter();

	GraphicsDevice() = default;
	GraphicsDevice(GraphicsDevice&&) = default;
	GraphicsDevice(GraphicsDevice&) = default;
public:
	static GraphicsDevice& Instance()
	{
		static GraphicsDevice instance;
		return instance;
	}
};