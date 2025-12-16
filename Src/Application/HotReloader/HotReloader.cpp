#include "HotReloader.h"
#include "Application/Application.h"
#include "AssetManager/AssetManager.h"
#include "DllLoader/DllLoader.h"

struct HotReloader::DllData
{
	DllData(const AssetManager::MetaData* _pData)
		:m_metaData(_pData)
	{
	
	}

	const AssetManager::MetaData* m_metaData;
	DllLoader m_loader;
};

class HotReloader::OnDllWriter :public AssetManager::IFileWriteHandler
{
public:
	OnDllWriter(HotReloader* _owner) :m_owner(_owner) {};

private:
	void OnWrite(const AssetManager::MetaData& _srcFile) override
	{
		auto& list = m_owner->m_dllIDList;
		auto it = std::ranges::find_if(list.begin(), list.end(), [&_srcFile](auto& _it) { return _it->m_metaData == &_srcFile; });

		if (it != list.end())
		{
			m_owner->m_dirtyDll.push_back(*it);
		}
	}

	HotReloader* m_owner;
};

HotReloader::HotReloader()
{
	Application::Instance().GetAssetManager()->AddListener(std::make_shared<OnDllWriter>(this));
}

HotReloader::~HotReloader()
{
	while (!m_dllIDList.empty())
	{
		auto dllData = m_dllIDList.back();
		dllData->m_loader.Release();

		delete dllData;
		dllData = nullptr;

		m_dllIDList.pop_back();
	}
}

void HotReloader::Update()
{
	if (m_dirtyDll.empty())return;
	Reload();
}

void HotReloader::Register(const std::filesystem::path& _srcPath)
{
	auto& registry = Application::Instance().GetAssetManager()->GetLibrary();
	auto it = std::find_if(registry.begin(), registry.end(), [&_srcPath](auto& metaData) { return _srcPath == metaData.second.filePath; });

	if (it != registry.end()) 
	{
		m_dllIDList.push_back(new DllData(&it->second));
	}
}
void HotReloader::Unregister(const std::filesystem::path& _srcPath)
{
	auto it = std::find_if(m_dllIDList.begin(), m_dllIDList.end(), [&_srcPath](auto& metaData) { return _srcPath == metaData->m_metaData->filePath; });
	if (it != m_dllIDList.end())
	{
		(**it).m_loader.Release();
		m_dllIDList.erase(it);
	}
}

void HotReloader::Reload()
{
	for (auto& it : m_dirtyDll)
	{
		it->m_loader.Release();
		it->m_loader.Load(it->m_metaData->filePath.c_str());
	}

	m_dirtyDll.clear();
}
