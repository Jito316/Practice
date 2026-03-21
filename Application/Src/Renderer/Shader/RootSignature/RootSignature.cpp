#include "RootSignature.h"
#include "../../Direct3D.h"

void RootSignature::Create(Direct3D* _pDevice, const std::vector<RangeType>& _types)
{
	m_pDevice = _pDevice;

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	int rangeCount = (int)_types.size();

	std::vector<D3D12_ROOT_PARAMETER> rootParams(rangeCount);
	std::vector<D3D12_DESCRIPTOR_RANGE> ranges(rangeCount);

	int cbvCount = 0;
	int samplerCount = 0;
	int uavCount = 0;
	bool bSampler = false;

	for (int i = 0; i < rangeCount; ++i)
	{
		switch (_types[i])
		{
		case RangeType::CBV:
			CreateRange(ranges[i], RangeType::CBV, cbvCount);
			rootParams[i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParams[i].DescriptorTable.pDescriptorRanges = &ranges[i];
			rootParams[i].DescriptorTable.NumDescriptorRanges = 1;
			rootParams[i].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

			++cbvCount;
			break;
		case RangeType::SRV:
			CreateRange(ranges[i], RangeType::SRV, samplerCount);
			rootParams[i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParams[i].DescriptorTable.pDescriptorRanges = &ranges[i];
			rootParams[i].DescriptorTable.NumDescriptorRanges = 1;
			rootParams[i].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

			bSampler = true;
			++samplerCount;
			break;
		case RangeType::UAV:
			CreateRange(ranges[i], RangeType::UAV, uavCount);
			rootParams[i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParams[i].DescriptorTable.pDescriptorRanges = &ranges[i];
			rootParams[i].DescriptorTable.NumDescriptorRanges = 1;
			rootParams[i].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

			++uavCount;
			break;
		default:
			break;
		}
	}

	std::array<D3D12_STATIC_SAMPLER_DESC, 4> pStaticSamplerDescs = {};
	if (bSampler)
	{
		CreateStaticSampler(pStaticSamplerDescs[0], TextureAddressMode::Wrap, D3D12Filter::Point, 0);
		CreateStaticSampler(pStaticSamplerDescs[1], TextureAddressMode::Clamp, D3D12Filter::Point, 1);
		CreateStaticSampler(pStaticSamplerDescs[2], TextureAddressMode::Wrap, D3D12Filter::Linear, 2);
		CreateStaticSampler(pStaticSamplerDescs[3], TextureAddressMode::Clamp, D3D12Filter::Linear, 3);
	}

	rootSignatureDesc.pStaticSamplers = bSampler ? pStaticSamplerDescs.data() : nullptr;
	rootSignatureDesc.NumStaticSamplers = bSampler ? 4 : 0;
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams.data();
	rootSignatureDesc.NumParameters = (int)rootParams.size();

	ID3DBlob* pRootBlob = nullptr;
	ID3D10Blob* pErrirBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &pRootBlob, &pErrirBlob);

	if (FAILED(hr))
	{
		assert(false && "ルートシグネチャ初期化失敗");
	}

	hr = m_pDevice->GetDevice()->CreateRootSignature(0, pRootBlob->GetBufferPointer(), pRootBlob->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature));

	if (FAILED(hr))
	{
		assert(false && "ルートシグネチャ作成失敗");
	}

	pRootBlob->Release();
}

void RootSignature::CreateRange(D3D12_DESCRIPTOR_RANGE& _pRange, RangeType _type, int _count)
{
	switch (_type)
	{
	case RangeType::CBV:
		_pRange = {};
		_pRange.NumDescriptors = 1;
		_pRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		_pRange.BaseShaderRegister = _count;
		_pRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		break;
	case RangeType::SRV:
		_pRange = {};
		_pRange.NumDescriptors = 1;
		_pRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		_pRange.BaseShaderRegister = _count;
		_pRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		break;
	case RangeType::UAV:
		_pRange = {};
		_pRange.NumDescriptors = 1;
		_pRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		_pRange.BaseShaderRegister = _count;
		_pRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		break;
	default:
		break;
	}
}

void RootSignature::CreateStaticSampler(D3D12_STATIC_SAMPLER_DESC& _pSamplerDesc, TextureAddressMode _mode, D3D12Filter _filter, int _count)
{
	D3D12_TEXTURE_ADDRESS_MODE addressMode = _mode == TextureAddressMode::Wrap ? D3D12_TEXTURE_ADDRESS_MODE_WRAP : D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	D3D12_FILTER samplingFilter = _filter == D3D12Filter::Point ? D3D12_FILTER_MIN_MAG_MIP_POINT : D3D12_FILTER_MIN_MAG_MIP_POINT;

	_pSamplerDesc = {};
	_pSamplerDesc.AddressU = addressMode;
	_pSamplerDesc.AddressV = addressMode;
	_pSamplerDesc.AddressW = addressMode;
	_pSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	_pSamplerDesc.Filter = samplingFilter;
	_pSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	_pSamplerDesc.MinLOD = 0.0f;
	_pSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	_pSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	_pSamplerDesc.MaxAnisotropy = 16;
	_pSamplerDesc.ShaderRegister = _count;
}
