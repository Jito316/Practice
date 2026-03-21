#include "Direct3D.h"

#include <vector>
#include <assert.h>

#include "Window/WindowsWindow.h"
#include "Renderer/Heap/RTVHeap/RTVHeap.h"

#ifdef _DEBUG
static void EnableDebugLayer()
{
	ID3D12Debug* debugLayer = nullptr;
	auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));

	debugLayer->EnableDebugLayer();//デバックレイヤー有効化
	debugLayer->Release();//インターフェイス解放
}
#endif // _DEBUG


bool Direct3D::Initialize(WindowsWindow* _window)
{
	if (CreateFactory() == false)
	{
		return false;
	}

#ifdef _DEBUG
	EnableDebugLayer();
#endif

	if (CreateDevice() == false)
	{
		return false;
	}
	if (CreateCommandObjects() == false)
	{
		return false;
	}

	if (CreateSwapChain(_window) == false)
	{
		return false;
	}

	if (CreateRTVBuffer() == false)
	{
		return false;
	}

	if (CreateFence() == false)
	{
		return false;
	}

	return true;
}

void Direct3D::Finalize()
{
	if (m_rtvHeaps)m_rtvHeaps.reset();
	if (m_backBuffers.size())
	{
		for (auto& buffer : m_backBuffers)buffer.Reset();
		m_backBuffers.clear();
	}

	if (m_swapChain)m_swapChain.Reset();

	if (m_cmdQueue)m_cmdQueue.Reset();
	if (m_cmdAllocator)m_cmdAllocator.Reset();
	if (m_cmdList)m_cmdList.Reset();
	if (m_pFence)m_pFence.Reset();

	if (m_pAdapter)m_pAdapter.Reset();

	if (m_device)m_device.Reset();

	if (m_dxgiFactory)m_dxgiFactory.Reset();
}

void Direct3D::Prepare()
{
	//リソースバリア設定
	auto bbIdx = m_swapChain->GetCurrentBackBufferIndex();
	SetResourceBarrier(m_backBuffers[bbIdx].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	//書き込むレンダーターゲットの設定
	auto rtv = m_rtvHeaps->GetCPUHandle(bbIdx);
	m_cmdList->OMSetRenderTargets(1, &rtv, true, nullptr);

	//コマンドリストに命令
	float clearColor[] = { 0.0f,0.0f, 1.0f, 1.0f };
	m_cmdList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
}

void Direct3D::ScreenFlip()
{
	//リソースバリア設定
	auto bbIdx = m_swapChain->GetCurrentBackBufferIndex();
	SetResourceBarrier(m_backBuffers[bbIdx].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	//命令のクローズ
	m_cmdList->Close();

	//コマンドリストの実行
	ID3D12CommandList* cmdLists[] = { m_cmdList.Get() };
	m_cmdQueue->ExecuteCommandLists(1, cmdLists);

	//待機
	HANDLE fenveEvent = CreateEvent(nullptr, false, false, nullptr);
	m_cmdQueue->Signal(m_pFence.Get(), ++m_fenceVal);
	if (m_pFence->GetCompletedValue() != m_fenceVal)
	{
		//イベントハンドルの取得
		m_pFence->SetEventOnCompletion(m_fenceVal, fenveEvent);
		//イベントが発生するまで待ち続ける（INFINITE）
		WaitForSingleObject(fenveEvent, INFINITE);
		CloseHandle(fenveEvent);
	}

	//コマンドアロケーターとコマンドリストを初期化
	m_cmdAllocator->Reset();
	m_cmdList->Reset(m_cmdAllocator.Get(), nullptr);

	//フリップ
	m_swapChain->Present(1, 0);
}

void Direct3D::SetResourceBarrier(ID3D12Resource* _pResource, D3D12_RESOURCE_STATES _stateBefore, D3D12_RESOURCE_STATES _stateAfter)
{
	//リソースバリア設定
	D3D12_RESOURCE_BARRIER barrierDesc = {};
	barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;//遷移
	barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;//特に指定なし
	barrierDesc.Transition.pResource = _pResource;//バックバッファリソース
	barrierDesc.Transition.Subresource = 0;
	barrierDesc.Transition.StateBefore = _stateBefore;
	barrierDesc.Transition.StateAfter = _stateAfter;
	m_cmdList->ResourceBarrier(1, &barrierDesc);
}

bool Direct3D::CreateFactory()
{
#ifdef _DEBUG
	HRESULT hr = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_dxgiFactory));
#else
	HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory));
#endif

	if (FAILED(hr))
	{
		assert(false && "DXGIファクトリ作成失敗");
		return false;
	}

	return true;
}

bool Direct3D::CreateDevice()
{
	m_pAdapter = FindAdapter();
	if (m_pAdapter == nullptr)
	{
		return false;
	}

	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	for (auto lv : levels)
	{
		HRESULT hr = D3D12CreateDevice(
			m_pAdapter.Get()//nullptrだと使用するアダプター（グラフィックボード）を自動で選択
			, lv//最低限必要なフィーチャーレベル、使うグラボ次第では非対応もアリ（最大はD3D_FEATURE_LEVEL_12_1）
			, IID_PPV_ARGS(&m_device)//第3,4引数、リザルト型とリザルト
		);

		if (hr == S_OK)
		{
			break;
		}
	}

	if (m_device == nullptr)
	{
		assert(false && "D3D12デバイス作成失敗");
		return false;
	}

	return true;
}

bool Direct3D::CreateCommandObjects()
{
	HRESULT hr = m_device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT
		, IID_PPV_ARGS(&m_cmdAllocator)
	);

	if (hr != S_OK)
	{
		assert(false && "D3D12コマンドアロケーター作成失敗");
		return false;
	}

	hr = m_device->CreateCommandList(
		0
		, D3D12_COMMAND_LIST_TYPE_DIRECT
		, m_cmdAllocator.Get(), nullptr
		, IID_PPV_ARGS(&m_cmdList)
	);

	if (hr != S_OK)
	{
		assert(false && "D3D12コマンドコマンドリスト作成失敗");
		return false;
	}

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};

	//タイムアウトなし
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	//アダプターを1つしか使わないときは0でよい
	cmdQueueDesc.NodeMask = 0;

	//プライオリティは特に指定ナシ
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

	//コマンドリストと合わせる
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	//キュー作成
	hr = m_device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_cmdQueue));
	if (hr != S_OK)
	{
		assert(false && "D3D12コマンドコマンドキュー作成失敗");
		return false;
	}

	return true;
}

bool Direct3D::CreateSwapChain(WindowsWindow* _window)
{
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};

	swapchainDesc.Width = _window->GetWidth();
	swapchainDesc.Height = _window->GetHeight();
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.Stereo = false;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.BufferCount = 2;

	//バックバッファは伸び縮み可能
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;

	//フリップ後は速やかに破棄
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	//特に指定なし
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	//ウィンドウ⇔振りscreen切り替え可能
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hr = m_dxgiFactory->CreateSwapChainForHwnd(
		m_cmdQueue.Get()
		, _window->GetWindowHandle()
		, &swapchainDesc, nullptr, nullptr
		, (IDXGISwapChain1**)m_swapChain.ReleaseAndGetAddressOf()
	);

	if (hr != S_OK)
	{
		assert(false && "DXGIスワップチェーン作成失敗");
		return false;
	}

	return true;
}

bool Direct3D::CreateRTVBuffer()
{

	DXGI_SWAP_CHAIN_DESC swpDesc;
	HRESULT hr = m_swapChain->GetDesc(&swpDesc);
	if (hr != S_OK)
	{
		assert(false && "DXGIスワップチェーン設定取得失敗");
		return false;
	}

	m_backBuffers.resize(swpDesc.BufferCount);

	m_rtvHeaps = std::make_shared<RTVHeap>();
	if (m_rtvHeaps->Create(m_device.Get(), swpDesc.BufferCount) == false)
	{
		return false;
	}

	for (int i = 0;i < m_backBuffers.size();++i)
	{
		hr = m_swapChain->GetBuffer(i,IID_PPV_ARGS(&m_backBuffers[i]));
		if (hr != S_OK)
		{
			assert(false && "バッファ作成失敗");
			return false;
		}


		m_rtvHeaps->CreateRTV(m_backBuffers[i].Get());
	}

	return true;
}

bool Direct3D::CreateFence()
{
	HRESULT hr = m_device->CreateFence(m_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence));
	if (hr != S_OK)
	{
		assert(false && "フェンス作成失敗");
		return false;
	}
	return true;
}

Microsoft::WRL::ComPtr<IDXGIAdapter> Direct3D::FindAdapter()
{
	Microsoft::WRL::ComPtr<IDXGIAdapter> pSelectAdapter = nullptr;
	std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter>> pAdapters;
	std::vector<DXGI_ADAPTER_DESC> descs;

	//使用中のPCにあるGPUドライバーを検索して、あれば格納
	for (UINT index = 0; true; ++index)
	{
		pAdapters.push_back(nullptr);
		HRESULT ret = m_dxgiFactory->EnumAdapters(index, &pAdapters[index]);

		if (ret == DXGI_ERROR_NOT_FOUND) { break; }

		descs.push_back({});
		pAdapters[index]->GetDesc(&descs[index]);
	}

	GPUTier gpuTier = GPUTier::Kind;

	//優先度の高いGPUドライバーを使用
	for (int i = 0; i < descs.size(); ++i)
	{
		if (std::wstring(descs[i].Description).find(L"NVIDIA") != std::wstring::npos)
		{
			pSelectAdapter = pAdapters[i];
			break;
		}
		else if (std::wstring(descs[i].Description).find(L"Amd") != std::wstring::npos)
		{
			if (gpuTier > GPUTier::Amd)
			{
				pSelectAdapter = pAdapters[i];
				gpuTier = GPUTier::Amd;
			}
		}
		else if (std::wstring(descs[i].Description).find(L"Intel") != std::wstring::npos)
		{
			if (gpuTier > GPUTier::Intel)
			{
				pSelectAdapter = pAdapters[i];
				gpuTier = GPUTier::Intel;
			}
		}
		else if (std::wstring(descs[i].Description).find(L"Arm") != std::wstring::npos)
		{
			if (gpuTier > GPUTier::Arm)
			{
				pSelectAdapter = pAdapters[i];
				gpuTier = GPUTier::Arm;
			}
		}
		else if (std::wstring(descs[i].Description).find(L"Qualcomm") != std::wstring::npos)
		{
			if (gpuTier > GPUTier::Qualcomm)
			{
				pSelectAdapter = pAdapters[i];
				gpuTier = GPUTier::Qualcomm;
			}
		}
	}

	if (pSelectAdapter == nullptr)
	{
		assert(false && "指定したグラボが見つかりませんでした");
	}

	return pSelectAdapter;
}