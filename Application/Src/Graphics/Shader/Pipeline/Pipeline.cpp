#include "Pipeline.h"
#include "../RootSignature/RootSignature.h"

void Pipeline::SetRenderSettings(GraphicsDevice* _pDevice, RootSignature* _pRootSignature, const std::vector<InputLayout>& _inputLayouts, CullMode _cullMode, BlendMode _blendMode, PrimitiveTopologyType _topologyType)
{
	m_pDevice = _pDevice;
	m_pRootSignature = _pRootSignature;
	m_inputLayouts = _inputLayouts;
	m_cullMode = _cullMode;
	m_blendMode = _blendMode;
	m_topologyType = _topologyType;
}

void Pipeline::Create(const std::vector<ID3DBlob*>& _pBlobs, const std::vector<DXGI_FORMAT>& _formats, bool _isDepth, bool _isDepthMask, int _rtvCount, bool _isWireFrame)
{
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayouts;
	SetInputLayout(inputLayouts, m_inputLayouts);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineState = {};

	graphicsPipelineState.VS.pShaderBytecode = _pBlobs[0]->GetBufferPointer();
	graphicsPipelineState.VS.BytecodeLength = _pBlobs[0]->GetBufferSize();

	if (_pBlobs[1])
	{
		graphicsPipelineState.HS.pShaderBytecode = _pBlobs[1]->GetBufferPointer();
		graphicsPipelineState.HS.BytecodeLength = _pBlobs[1]->GetBufferSize();
	}

	if (_pBlobs[2])
	{
		graphicsPipelineState.DS.pShaderBytecode = _pBlobs[2]->GetBufferPointer();
		graphicsPipelineState.DS.BytecodeLength = _pBlobs[2]->GetBufferSize();
	}

	if (_pBlobs[3])
	{
		graphicsPipelineState.GS.pShaderBytecode = _pBlobs[3]->GetBufferPointer();
		graphicsPipelineState.GS.BytecodeLength = _pBlobs[3]->GetBufferSize();
	}

	graphicsPipelineState.PS.pShaderBytecode = _pBlobs[4]->GetBufferPointer();
	graphicsPipelineState.PS.BytecodeLength = _pBlobs[4]->GetBufferSize();

	graphicsPipelineState.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	graphicsPipelineState.RasterizerState.CullMode = static_cast<D3D12_CULL_MODE>(m_cullMode);

	if (_isWireFrame)
	{
		graphicsPipelineState.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	}
	else
	{
		graphicsPipelineState.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	}

	if (_isDepth)
	{
		graphicsPipelineState.RasterizerState.DepthClipEnable = true;
		graphicsPipelineState.DepthStencilState.DepthEnable = true;
		graphicsPipelineState.DepthStencilState.StencilEnable = false;

		if (_isDepthMask)
		{
			graphicsPipelineState.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		}
		else
		{
			graphicsPipelineState.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		}

		graphicsPipelineState.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		graphicsPipelineState.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	}
	else
	{
		graphicsPipelineState.RasterizerState.DepthClipEnable = false;
		graphicsPipelineState.DepthStencilState.DepthEnable = false;
		graphicsPipelineState.DepthStencilState.StencilEnable = false;
	}

	graphicsPipelineState.BlendState.AlphaToCoverageEnable = false;

	graphicsPipelineState.BlendState.IndependentBlendEnable = false;

	D3D12_RENDER_TARGET_BLEND_DESC blendDesc = {};
	SetBlendMode(blendDesc, m_blendMode);
	graphicsPipelineState.BlendState.RenderTarget[0] = blendDesc;


	graphicsPipelineState.InputLayout.pInputElementDescs = inputLayouts.data();
	graphicsPipelineState.InputLayout.NumElements = (int)inputLayouts.size();
	graphicsPipelineState.NumRenderTargets = _rtvCount;

	graphicsPipelineState.PrimitiveTopologyType = (_pBlobs[3] && _pBlobs[4]) ? D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH : static_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE>(m_topologyType);

	graphicsPipelineState.NumRenderTargets = _rtvCount;

	for (int i = 0; i < _rtvCount; ++i)
	{
		graphicsPipelineState.RTVFormats[i] = _formats[i];
	}

	graphicsPipelineState.SampleDesc.Count = 1;
	graphicsPipelineState.pRootSignature = m_pRootSignature->GetRootSignature();

	HRESULT hr = m_pDevice->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineState, IID_PPV_ARGS(&m_pPipelineState));
	if (FAILED(hr))
	{
		assert(false && "パイプラインステートの作成に失敗しました");
		return;
	}
}

void Pipeline::SetInputLayout(std::vector<D3D12_INPUT_ELEMENT_DESC>& _inputElemnts, const std::vector<InputLayout>& _inputLayouts)
{
	for (int i = 0; i < (int)_inputLayouts.size(); ++i)
	{
		switch (_inputLayouts[i])
		{
		case InputLayout::POSITION:
			_inputElemnts.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
			break;
		case InputLayout::TEXCOORD:
			_inputElemnts.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
			break;
		case InputLayout::NORMAL:
			_inputElemnts.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
			break;
		case InputLayout::COLOR:
			_inputElemnts.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
			break;
		case InputLayout::TANGENT:
			_inputElemnts.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
			break;
		case InputLayout::SKININDEX:
			_inputElemnts.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "SKININDEX", 0, DXGI_FORMAT_R16G16B16A16_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
			break;
		case InputLayout::SKINWEIGHT:
			_inputElemnts.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "SKINWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
			break;
		default:
			break;
		}
	}
}

void Pipeline::SetBlendMode(D3D12_RENDER_TARGET_BLEND_DESC& _blendDesc, BlendMode _blendMode)
{
	_blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	_blendDesc.BlendEnable = true;

	switch (_blendMode)
	{
	case BlendMode::Add:
		_blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		_blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		_blendDesc.DestBlend = D3D12_BLEND_ONE;

		_blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		_blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		_blendDesc.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		_blendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
		break;
	case BlendMode::Appha:
		_blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		_blendDesc.SrcBlend = D3D12_BLEND_DEST_ALPHA;
		_blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		_blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		_blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		_blendDesc.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		_blendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
		break;
	default:
		break;
	}
}
