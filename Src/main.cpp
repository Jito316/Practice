// Practice.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <thread>
#include "Application/Application.h"
#include "AssetManager/AssetManager.h"
#include "DllLoader/DllLoader.h"
#include "Application/HotReloader/HotReloader.h"

//#include "ICommand.h"

class OnWriteCallBack :public AssetManager::IFileWriteHandler
{
	void OnWrite(const AssetManager::MetaData& _meta)override
	{
		std::cout << "書き込み！:" << _meta.filePath << std::endl;
	}
};

int main()
{
	DllLoader dllLoader;
	HotReloader hotreloader;
	auto& assetMgr = *Application::Instance().GetAssetManager();

	assetMgr.AddListener(std::make_shared<OnWriteCallBack>());

	for (auto& [key, vale] : assetMgr.GetLibrary())
	{
		std::filesystem::path path(vale.filePath);
		if (".dll" == path.extension())
		{
			hotreloader.Register(path);
		}
	}

	while (!Application::Instance().IsEnd())
	{
		hotreloader.Update();

		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		{
			Application::Instance().End();
		}
	}
}
