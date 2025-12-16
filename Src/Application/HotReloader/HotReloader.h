#pragma once
#include <list>
#include <filesystem>
#include <shared_mutex>

class Guid;

class HotReloader
{
public:
	HotReloader();
	~HotReloader();

	void Update();

	void Register(const std::filesystem::path& _srcPath);
	void Unregister(const std::filesystem::path& _srcPath);
	void Reload();

private:
	class OnDllWriter;
	struct DllData;
	std::list<DllData*> m_dllIDList;

	std::list<DllData*> m_dirtyDll;
};