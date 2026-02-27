#include "Direct3D.h"

#include <vector>
#include <assert.h>

#include "Window/WindowsWindow.h"

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
#ifdef _DEBUG
	EnableDebugLayer();
	HRESULT hr = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_dxgiFactory));
#else
	HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory));
#endif

	if (hr != S_OK)
	{
		assert(false && "DXGIファクトリ作成失敗");
		return false;
	}

	if (CreateDevice() == false)
	{
		assert(false && "D3D12デバイス作成失敗");
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

	if (CreateBuffer() == false)
	{
		return false;
	}

	hr = m_device->CreateFence(m_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence));
	if (hr != S_OK)
	{
		assert(false && "フェンス作成失敗");
		return false;
	}

	m_fenveEvent = CreateEvent(nullptr, false, false, nullptr);

	return true;
}

void Direct3D::Finalize()
{
	//イベントハンドルを閉じる
	CloseHandle(m_fenveEvent);
	m_pFence.Reset();

	m_rtvHeaps.Reset();

	m_swapChain.Reset();

	m_cmdQueue.Reset();

	m_cmdAllocator.Reset();
	m_cmdList.Reset();

	m_device.Reset();
	m_dxgiFactory.Reset();
}

void Direct3D::Render()
{
	auto bbIdx = m_swapChain->GetCurrentBackBufferIndex();

	m_cmdAllocator->Reset();//キュークリア
	m_cmdList->Reset(m_cmdAllocator.Get(), nullptr);//再びコマンドリストを溜める準備

	//リソースバリア設定
	D3D12_RESOURCE_BARRIER barrierDesc = {};
	barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;//遷移
	barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;//特に指定なし
	barrierDesc.Transition.pResource = m_backBuffers[bbIdx];//バックバッファリソース
	barrierDesc.Transition.Subresource = 0;
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_cmdList->ResourceBarrier(1, &barrierDesc);

	//書き込むレンダーターゲットの設定
	auto rtv = m_rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtv.ptr += bbIdx * m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_cmdList->OMSetRenderTargets(1, &rtv, true, nullptr);

	//コマンドリストに命令
	float clearColor[] = { 0.0f,0.0f, 1.0f, 1.0f };
	m_cmdList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);

	//リソースバリア設定
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	m_cmdList->ResourceBarrier(1, &barrierDesc);

	//命令のクローズ
	m_cmdList->Close();

	//コマンドリストの実行
	ID3D12CommandList* cmdLists[] = { m_cmdList.Get() };
	m_cmdQueue->ExecuteCommandLists(1, cmdLists);

	//待機
	m_cmdQueue->Signal(m_pFence.Get(), ++m_fenceVal);
	if (m_pFence->GetCompletedValue() != m_fenceVal)
	{
		//イベントハンドルの取得
		m_pFence->SetEventOnCompletion(m_fenceVal, m_fenveEvent);
		//イベントが発生するまで待ち続ける（INFINITE）
		WaitForSingleObject(m_fenveEvent, INFINITE);
	}

	//フリップ
	m_swapChain->Present(1, 0);
}

bool Direct3D::CreateDevice()
{
	auto tmpAdapter = FindAdapter();
	if (tmpAdapter == nullptr)
	{
		assert(false && "指定したグラボが見つかりませんでした");
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
			tmpAdapter//nullptrだと使用するアダプター（グラフィックボード）を自動で選択
			, lv//最低限必要なフィーチャーレベル、使うグラボ次第では非対応もアリ（最大はD3D_FEATURE_LEVEL_12_1）
			, IID_PPV_ARGS(&m_device)//第3,4引数、リザルト型とリザルト
		);

		if (hr == S_OK)
		{
			break;
		}
	}

	return m_device != nullptr;
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

	m_cmdList->Close();

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

	IDXGISwapChain1* swapchain = nullptr;
	HRESULT hr = m_dxgiFactory->CreateSwapChainForHwnd(
		m_cmdQueue.Get()
		, _window->GetWindowHandle()
		, &swapchainDesc, nullptr, nullptr
		, &swapchain
	);

	if (hr != S_OK)
	{
		assert(false && "DXGIスワップチェーン作成失敗");
		return false;
	}

	m_swapChain = Microsoft::WRL::ComPtr<IDXGISwapChain4>((IDXGISwapChain4*)swapchain);

	return true;
}

bool Direct3D::CreateBuffer()
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;//レンダーターゲットビューなのでRTV
	heapDesc.NodeMask = 0;//使うGPUの指定（一つだけでなら0で良し！
	heapDesc.NumDescriptors = 2;//表裏二つ
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	HRESULT hr = m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_rtvHeaps));
	if (hr != S_OK)
	{
		assert(false && "ビュー用のメモリ領域確保失敗");
		return false;
	}

	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	hr = m_swapChain->GetDesc(&swcDesc);

	if (hr != S_OK)
	{
		assert(false && "スワップチェーンの設定取得失敗");
		return false;
	}

	m_backBuffers.resize(swcDesc.BufferCount);
	for (size_t index = 0; index < swcDesc.BufferCount; ++index)
	{
		hr = m_swapChain->GetBuffer(index, IID_PPV_ARGS(&m_backBuffers[index]));
		if (hr != S_OK)
		{
			assert(false && "バックバッファの取得失敗");
			return false;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += index * m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		m_device->CreateRenderTargetView(m_backBuffers[index], nullptr, handle);
	}

	return true;
}

IDXGIAdapter* Direct3D::FindAdapter()
{
	//アダプターの列挙用
	std::vector<IDXGIAdapter*>adapters;
	//ここに特定の名前を持つアダプターオブジェクトが入る
	IDXGIAdapter* tmpAdapter = nullptr;

	for (int i = 0;
		m_dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND;
		++i)
	{
		adapters.push_back(tmpAdapter);
	}

	for (auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);//アダプターの説明オブジェクト取得

		std::wstring strDesc = adesc.Description;

		if (strDesc.find(L"NVIDIA") != std::string::npos)
		{
			tmpAdapter = adpt;
		}
		else 
		{
			adpt->Release();
		}
	}

	return tmpAdapter;
}