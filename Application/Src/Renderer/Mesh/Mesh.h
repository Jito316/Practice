#pragma once
class Direct3D;

struct Vertex
{
	Vertex(Math::Vector3 _position, Math::Vector2 _uv) :Position(_position), UV(_uv) {}

	Math::Vector3 Position = {};
	Math::Vector2 UV = {};
};

class Mesh
{
public:
	bool Create(Direct3D* _pDevice);

	void DrawInstanced() const;

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pVBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_vbView = {};
	std::vector<Vertex> m_vertices;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_pIBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW m_ibView = {};
	std::vector<UINT> m_indeices;

	Direct3D* m_pDevice = nullptr;
};