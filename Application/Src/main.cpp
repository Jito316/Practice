#ifdef _DEBUG
#include <iostream>
#endif
#include "Window/WindowsWindow.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	WindowsWindow window;
	window.Setup();

	while (window.IsEnd() == false)
	{
		window.Execute();
	}

	window.Shutdown();
	return 0;
}