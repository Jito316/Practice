#pragma once

class DllLoader
{
public:
	~DllLoader();

	bool Load(const char* dllPath);
	void Release();

	void* GetFunction(const char* functionName);
private:
	struct hModuleImple;
	hModuleImple* m_phModuleImple = nullptr;
};
