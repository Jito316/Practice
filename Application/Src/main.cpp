#ifdef _DEBUG
#include <iostream>
#endif
#include "Window/WindowsWindow.h"
#include "Renderer/Direct3D.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	CoInitializeEx(nullptr,COINIT_MULTITHREADED);

	WindowsWindow window;
	auto& d3d = Direct3D::Instance();
	window.Setup(1280, 720);
	if (d3d.Initialize(&window) == false) 
	{
		window.End();
	}

	while (window.IsEnd() == false)
	{
		window.Execute();
		d3d.Render();
	}

	d3d.Finalize();
	window.Shutdown();

	CoUninitialize();

	return 0;
}