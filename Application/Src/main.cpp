#ifdef _DEBUG
#include <iostream>
#endif
#include "Window/WindowsWindow.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/Shader/Shader.h"
#include "Graphics/Texture/Texture.h"
#include "Graphics/Heap/CBVSRVUAVHeap/CBVSRVUAVHeap.h"
#include "Graphics/CBufferAllocator/CBufferAllocator.h"
#include "Graphics/CBufferAllocator/CBufferData/CBufferData.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	(void)CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	WindowsWindow window;
	auto& d3d = GraphicsDevice::Instance();
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
		renderingSetting.InputLayout = { InputLayout::POSITION,InputLayout::TEXCOORD };
		renderingSetting.Formats = { DXGI_FORMAT_R8G8B8A8_UNORM };
		renderingSetting.IsDepth = false;
		renderingSetting.IsDepthMask = false;

		Shader shader;
		shader.Create(&d3d, L"BasicShader", renderingSetting, { RangeType::CBV,RangeType::SRV });

		Texture sampleTex;
		sampleTex.Load(&d3d, "Assets/Texture/IMG_3724.png");

		CBufferData::Camera cbCamera;
		cbCamera.mView = Math::Matrix::CreateTranslation(0, 0, 3);
		cbCamera.mProj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60.f), 1280.0f / 720.0f, 0.01f, 1000.0f);



		while (window.IsEnd() == false)
		{
			window.Execute();


			d3d.Prepare();
			d3d.GetCBVSRVUAVHeap()->SetHeap();

			d3d.GetCBufferAllocator()->ResetCurrentUseNumber();

			shader.Begin(1280, 720);

			sampleTex.Set(shader.GetCBVCount() + sampleTex.GetSRVNumber());

			d3d.GetCBufferAllocator()->BindAndAttachData(0, cbCamera);

			shader.DrawMesh(mesh);

			d3d.ScreenFlip();
		}
	}

	d3d.Finalize();
	window.Shutdown();

	CoUninitialize();

	return 0;
}