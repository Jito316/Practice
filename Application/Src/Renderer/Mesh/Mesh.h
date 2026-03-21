#pragma once
class Direct3D;

class Mesh
{
public:
	bool Create(Direct3D* _pDevice);

	void DrawInstanced() const;

private:
	std::array<Math::Vector3, 3> m_vertices;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_pVBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_vbView;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_pIBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW m_ibView;

	Direct3D* m_pDevice = nullptr;
};