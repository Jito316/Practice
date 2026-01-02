#include "Direct3D.h"

#include <vector>
#include <assert.h>

#include "Window/WindowsWindow.h"

bool Direct3D::Initialize(WindowsWindow* _window)
{
	HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory));
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
		assert(false && "DXGIスワップチェーン作成失敗");
		return false;
	}

	return true;
}

void Direct3D::Finalize()
{
	m_swapChain.Reset();

	m_cmdQueue.Reset();
	m_cmdList.Reset();
	m_cmdAllocator.Reset();

	m_device.Reset();
	m_dxgiFactory.Reset();
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

	if (hr == S_OK)
	{
		m_swapChain = Microsoft::WRL::ComPtr<IDXGISwapChain4>((IDXGISwapChain4*)swapchain);
		return true;
	}

	return false;
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
			break;
		}
	}

	return tmpAdapter;
}
