#include "DllLoader.h"
#include <iostream>
#include <libloaderapi.h>

struct DllLoader::hModuleImple
{
	HMODULE hModule = nullptr;
};

DllLoader::~DllLoader()
{
	if (m_phModuleImple)
	{
		Release();
	}
}

bool DllLoader::Load(const char* dllPath)
{
	if (m_phModuleImple) {
		std::cout << "loaded to DllLoader: " << dllPath << std::endl;
		return false;
	}

	m_phModuleImple = new hModuleImple;

	m_phModuleImple->hModule = LoadLibraryA(dllPath);
	if (!m_phModuleImple->hModule) {
		std::cout << "Failed to load DLL: " << dllPath << std::endl;
		return false;
	}
	return true;
}

void DllLoader::Release()
{
	if (m_phModuleImple)
	{
		FreeLibrary(m_phModuleImple->hModule);
		delete m_phModuleImple;
	}
	m_phModuleImple = nullptr;
}

void* DllLoader::GetFunction(const char* functionName)
{
	auto procaddress = GetProcAddress(m_phModuleImple->hModule, functionName);
	if (!procaddress)
	{
		std::cout << "Failed to Get Function: " << functionName << std::endl;
	}
	return procaddress;
}