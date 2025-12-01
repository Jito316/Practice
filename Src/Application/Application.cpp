#include "Application.h"
#include "AssetManager/AssetManager.h"
#include <iostream>

class CallbackWirte :public AssetManager::IFileWriteEvent 
{
	void  OnWrite(const std::filesystem::path& _path) 
	{
		std::cout << "更新：" << _path << std::endl;
	}
};

Application::Application() 
{
	m_assetManager = new AssetManager();
	m_assetManager->CreateMetaFileForAllFiles();
	m_assetManager->CreateAddressablesList();
	m_assetManager->AddListener(std::make_shared<CallbackWirte>());
}