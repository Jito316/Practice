#ifdef _DEBUG
#include <iostream>
#endif
#include "Window/WindowsWindow.h"
#include "Renderer/Direct3D.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	WindowsWindow window;
	auto& d3d = Direct3D::Instance();
	window.Setup(1280, 720);
	d3d.Initialize(&window);

	while (window.IsEnd() == false)
	{
		window.Execute();
		d3d.Render();
	}

	d3d.Finalize();
	window.Shutdown();
	return 0;
}