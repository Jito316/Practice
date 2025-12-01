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

int main()
{
	
	while (!Application::Instance().IsEnd())
	{
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		{
			Application::Instance().End();
		}
	}

	/*
	std::string path = mgr.GetFilePathFromAddressableName("addName");
	std::ifstream ifs(path);
	if (ifs.fail() == false)
	{
		std::cout << "AddressableNameでファイルが読み込めた！" << std::endl;
	}
	*/
}
