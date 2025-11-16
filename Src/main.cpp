// Practice.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <thread>
#include <Windows.h>
#include "Application/Application.h"
#include "AssetManager/AssetManager.h"

void print_datetime(const char* name, std::filesystem::file_time_type tp)
{
	// local_timeは、システム時間のエポックからの経過時間によって構築できる
	auto temp = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch());
	std::cout << name << "\t: " << tp << std::endl;
}


void FileUpdate()
{
	while (!Application::Instance().IsEnd())
	{
		std::cout << "ファイル監視" << std::endl;
		auto& mgr = *Application::Instance().GetAssetManager();
		auto& library = mgr.GetLibrary();

		for (auto& [ID, Path] : library)
		{
			std::filesystem::file_time_type temp = std::filesystem::last_write_time(Path.filePath);
			print_datetime(Path.filePath.c_str(), temp);
		}

		Application::Instance().End();
	}
}

int main()
{
	std::thread thread(FileUpdate);
	while (!Application::Instance().IsEnd())
	{
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		{
			Application::Instance().End();
		}
	}
	thread.join();

	/*
	std::string path = mgr.GetFilePathFromAddressableName("addName");
	std::ifstream ifs(path);
	if (ifs.fail() == false)
	{
		std::cout << "AddressableNameでファイルが読み込めた！" << std::endl;
	}
	*/
}
