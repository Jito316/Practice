#pragma once

class DllLoader
{
public:
	~DllLoader();

	bool Load(const char* dllPath);
	void Release();

	void* GetFunction(const char* dllPath);
private:
	struct hModuleImple;
	hModuleImple* m_phModuleImple = nullptr;
};
