#include <iostream>
#include <string>

#include "HotReloader.h"
#include "Application/Application.h"
#include "AssetManager/AssetManager.h"
#include "DllLoader/DllLoader.h"
#include "IModule.h"

class HotReloader::DllData
{
public:
	typedef IModule* (*CraeteFunction)(void);
	typedef void(*DeleteFunction)(IModule*&);

	DllData(const AssetManager::MetaData* _pData)
		:m_metaData(_pData)
	{

	}

	bool Load()
	{
		std::filesystem::path srcFile(m_metaData->filePath);
		std::filesystem::path copyFile(srcFile.filename().string() + ".copydll");
		if (!std::filesystem::copy_file(m_metaData->filePath.c_str(), copyFile))
		{
			std::cout << "コピー失敗:"<< srcFile.filename() << std::endl;
			return false;
		}

		if (m_loader.Load(copyFile.string().c_str()))
		{
			if (auto creator = (CraeteFunction)m_loader.GetFunction("CreateInstance"))
			{
				m_module = creator();

				m_module->Initialize();

				return true;
			}
		}

		std::filesystem::remove(copyFile);

		return false;
	}

	void Release()
	{
		if (m_module)
		{
			m_module->Finalize();

			auto deleter = (DeleteFunction)m_loader.GetFunction("DeleteInstance");
			deleter(m_module);
		}

		m_loader.Release();

		std::filesystem::path srcFile(m_metaData->filePath);
		std::filesystem::path copyFile(srcFile.filename().string() + ".copydll");
		std::filesystem::remove(copyFile);
	}

	void Update()
	{
		if (m_module)
		{
			m_module->Update();
		}
	}

private:
	const AssetManager::MetaData* m_metaData;
	DllLoader m_loader;
	IModule* m_module = nullptr;

public:
	bool operator == (const std::filesystem::path& _srcPath)
	{
		return m_metaData->filePath == _srcPath;
	}

	bool operator == (const AssetManager::MetaData* _pData)
	{
		return m_metaData == _pData;
	}

};

class HotReloader::OnDllWriter :public AssetManager::IFileWriteHandler
{
public:
	OnDllWriter(HotReloader* _owner) :m_owner(_owner) {};

private:
	void OnWrite(const AssetManager::MetaData& _srcFile) override
	{
		auto& list = m_owner->m_dllIDList;
		auto it = std::ranges::find_if(list.begin(), list.end(), [&_srcFile](auto& _it) { return *_it == &_srcFile; });

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
		dllData->Release();

		delete dllData;
		dllData = nullptr;

		m_dllIDList.pop_back();
	}
}

void HotReloader::Update()
{
	for (auto& it : m_dllIDList)
	{
		it->Update();
	}

	if (m_dirtyDll.empty())return;
	Reload();
}

void HotReloader::Register(const std::filesystem::path& _srcPath)
{
	auto& registry = Application::Instance().GetAssetManager()->GetLibrary();
	auto it = std::find_if(registry.begin(), registry.end(), [&_srcPath](auto& metaData) { return _srcPath == metaData.second.filePath; });

	if (it != registry.end())
	{
		auto temp = new DllData(&it->second);
		if (temp->Load())
		{
			m_dllIDList.push_back(temp);
		}
		else
		{
			temp->Release();
			delete temp;
			temp = nullptr;
		}
	}
}
void HotReloader::Unregister(const std::filesystem::path& _srcPath)
{
	auto it = std::find_if(m_dllIDList.begin(), m_dllIDList.end(), [&_srcPath](auto& metaData) { return *metaData == _srcPath; });
	if (it != m_dllIDList.end())
	{
		(**it).Release();
		m_dllIDList.erase(it);
	}
}

void HotReloader::Reload()
{
	for (auto& it : m_dirtyDll)
	{
		it->Release();
		it->Load();
	}

	m_dirtyDll.clear();
}
