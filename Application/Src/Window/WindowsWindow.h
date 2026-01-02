#pragma once

class WindowsWindow
{
public:
	~WindowsWindow()
	{
		Shutdown();
	}

	bool IsEnd() const
	{
		return m_isEnd;
	}

	void End()
	{
		m_isEnd = true;
	}

	void Setup(int _width,int _height);
	void Execute();
	void Shutdown();

	HWND GetWindowHandle() const
	{
		return m_hwnd;
	}

	HINSTANCE GetInstanceHandle() const
	{
		return GetModuleHandle(0);
	}

	int GetWidth()const { return m_windowSize.width; }
	int GetHeight()const { return m_windowSize.height; }

private:
	HWND m_hwnd = nullptr;

	std::wstring m_className;

	bool m_isEnd = false;

	struct { int width = 0, height = 0; }m_windowSize;
};