#ifdef _DEBUG
#include <iostream>
#endif
#include "Window/WindowsWindow.h"
#include "Renderer/Direct3D.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	WindowsWindow window;
	window.Setup(1280, 720);
	Direct3D::Instance().Initialize(&window);

	while (window.IsEnd() == false)
	{
		window.Execute();
	}

	Direct3D::Instance().Finalize();
	window.Shutdown();
	return 0;
}