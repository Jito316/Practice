#include "Application.h"
#include "AssetManager/AssetManager.h"
#include <iostream>

class CallbackWirte :public AssetManager::IFileWriteHandler 
{
	void  OnWrite(const AssetManager::MetaData& _path)
	{
		std::cout << "更新：" << _path.filePath << std::endl;
	}
};

Application::Application() 
{
	m_assetManager = new AssetManager();
	m_assetManager->CreateMetaFileForAllFiles();
	m_assetManager->CreateAddressablesList();
	m_assetManager->AddListener(std::make_shared<CallbackWirte>());
}