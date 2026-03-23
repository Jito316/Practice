#pragma once

enum class CullMode
{
	None = D3D12_CULL_MODE_NONE,
	Front = D3D12_CULL_MODE_FRONT,
	Back = D3D12_CULL_MODE_BACK,
};

enum class BlendMode
{
	Add,
	Appha,
};

enum class InputLayout
{
	POSITION,
	TEXCOORD,
	NORMAL,
	TANGENT,
	COLOR,
	SKININDEX,
	SKINWEIGHT,
};

enum class PrimitiveTopologyType
{
	Undefined = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED,
	Point = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,
	Line = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
	Triangle = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
	Patch = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH,
};

class Direct3D;
class RootSignature;

class Pipeline
{
public:
	void SetRenderSettings(Direct3D* _pDevice, RootSignature* _pRootSignature, const std::vector<InputLayout>& _inputLayouts, CullMode _cullMode, BlendMode _blendMode, PrimitiveTopologyType _topologyType);

	void Create(const std::vector<ID3DBlob*>& _pBlobs, const std::vector<DXGI_FORMAT>& _formats, bool _isDepth, bool _isDepthMask, int _rtvCount, bool _isWireFrame);

	ID3D12PipelineState* GetPipeline() const { return m_pPipelineState.Get(); }
	PrimitiveTopologyType GetTopologyType() const { return m_topologyType; }

private:
	void SetInputLayout(std::vector<D3D12_INPUT_ELEMENT_DESC>& _inputElemnts, const std::vector<InputLayout>& _inputLayouts);
	void SetBlendMode(D3D12_RENDER_TARGET_BLEND_DESC& _blendDesc, BlendMode _blendMode);

	std::vector<InputLayout> m_inputLayouts;
	CullMode m_cullMode = CullMode::Back;
	BlendMode m_blendMode = BlendMode::Appha;
	PrimitiveTopologyType m_topologyType = PrimitiveTopologyType::Triangle;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pPipelineState;

	Direct3D* m_pDevice = nullptr;
	RootSignature* m_pRootSignature = nullptr;
};