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

//#include "ICommand.h"

int main()
{
	DllLoader dllLoader;
	//ICommand* command = nullptr;

	while (!Application::Instance().IsEnd())
	{
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		{
			Application::Instance().End();
		}
	}
}
