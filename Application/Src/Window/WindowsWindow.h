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

	void Setup();
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

private:
	HWND m_hwnd;

	std::wstring m_className;

	bool m_isEnd = false;
};