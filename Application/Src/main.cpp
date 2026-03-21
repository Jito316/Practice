#ifdef _DEBUG
#include <iostream>
#endif
#include "Window/WindowsWindow.h"
#include "Renderer/Direct3D.h"
#include "Renderer/Shader/Shader.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	WindowsWindow window;
	auto& d3d = Direct3D::Instance();
	window.Setup(1280, 720);

	if (d3d.Initialize(&window) == false)
	{
		window.End();
	}
	else
	{

		Mesh mesh;
		mesh.Create(&d3d);

		RenderingSetting renderingSetting = {};
		renderingSetting.InputLayout = { InputLayout::POSITION };
		renderingSetting.Formats = { DXGI_FORMAT_R8G8B8A8_UNORM };
		renderingSetting.IsDepth = false;
		renderingSetting.IsDepthMask = false;

		Shader shader;
		shader.Create(&d3d, L"BasicShader", renderingSetting, {});

		while (window.IsEnd() == false)
		{
			window.Execute();


			d3d.Prepare();

			shader.Begin(1280, 720);
			shader.DrawMesh(mesh);

			d3d.ScreenFlip();
		}
	}

	d3d.Finalize();
	window.Shutdown();

	CoUninitialize();

	return 0;
}