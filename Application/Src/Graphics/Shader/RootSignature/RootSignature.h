#pragma once

enum class RangeType
{
	CBV,
	SRV,
	UAV,
};

enum class TextureAddressMode 
{
	Wrap,
	Clamp,
};

enum class D3D12Filter
{
	Point,
	Linear,
};

class RootSignature
{
public:
	void Create(GraphicsDevice* _pDevice,const std::vector<RangeType>& _types);

	ID3D12RootSignature* GetRootSignature() { return m_pRootSignature.Get(); }

private:
	void CreateRange(D3D12_DESCRIPTOR_RANGE& _pRange,RangeType _type,int _count);
	void CreateStaticSampler(D3D12_STATIC_SAMPLER_DESC& _pSampler,TextureAddressMode _mode,D3D12Filter _filter,int _count);

	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_pRootSignature;

	GraphicsDevice* m_pDevice = nullptr;
};