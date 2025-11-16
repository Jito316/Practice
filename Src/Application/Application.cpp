#include "Application.h"
#include "AssetManager/AssetManager.h"

Application::Application() 
{
	m_assetManager = new AssetManager();
	m_assetManager->CreateMetaFileForAllFiles();
	m_assetManager->CreateAddressablesList();
}