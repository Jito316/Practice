#pragma once
class AssetManager;

class Application 
{
public:
	static Application& Instance()
	{
		static Application instance;
		return instance;
	}

	bool IsEnd() const { return m_isEnd; }
	void End() { m_isEnd = true; }
	AssetManager* GetAssetManager() { return m_assetManager; };

private:
	bool m_isEnd = false;
	AssetManager* m_assetManager = nullptr;

	Application();
};