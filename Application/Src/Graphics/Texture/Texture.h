#pragma once

class Texture 
{
public:
	bool Load(GraphicsDevice* _pDevice, const std::string& _filepath);

	void Set(int _index);

	int GetSRVNumber()const { return m_srvNumber; }

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pBuffer;
	int m_srvNumber = 0;

	GraphicsDevice* m_pDevice = nullptr;
};