#pragma once

// std
#include <filesystem>
#include <fstream>
#include <list>
#include <unordered_map>
#include <thread>
#include <shared_mutex>

// json
#include <nlohmann/json.hpp>

class AssetManager
{
public:
	AssetManager() : m_thread(std::bind(&AssetManager::FileUpdate, this)) {}
	~AssetManager() { m_thread.join(); }

	struct MetaData
	{
		std::string filePath;
		long long writeTime = 0;
	};

	class IFileWriteEvent 
	{
	public:
		virtual void OnWrite(const std::filesystem::path&) = 0;
	};

	// Metaファイルを探してApplicationで使用するリストを作成する
	void CreateAddressablesList();

	// Assetsフォルダ以下をクロールして、Metaファイルを更新していく
	void CreateMetaFileForAllFiles();

	// ファイル一つに対してのMetaファイル作成
	nlohmann::json CreateMetaFileForFile(const std::filesystem::path& srcFile);

	// 作成したMetaファイルを全部削除
	void DeleteAllMetaFiles();

	// AddressableNameを指定してファイルパスの作成
	std::string GetFilePathFromAddressableName(const std::string& addressableName);

	// 固定のファイルパスやらファイル名やら
	std::string _assetFilePass = "./Assets/";   // Assetファイルの先頭ディレクトリ
	std::string _metaFileExtentionName = ".kdfwmeta"; // 作成するメタファイルの拡張子
	std::string _logFileName = "AssetManager.log";  // Log保存場所

	const std::unordered_map<std::string, MetaData>& GetLibrary() const { return  _addressables; }
	void AddListener(std::shared_ptr<IFileWriteEvent> _event) { m_fileWriteEvents.push_back(_event); }

private:
	// 対応する拡張子
	std::list<std::string> _supportedExtensions;
	// AddressableNameとメタ情報のリスト
	std::unordered_map<std::string, MetaData> _addressables;

	std::string AddressableName(const std::filesystem::path& srcFile) const;
	// サポートしているファイル形式か確認する
	bool IsSupportedFile(const std::filesystem::path& filePath);

	std::thread m_thread;
	void FileUpdate();

	std::list<std::shared_ptr<IFileWriteEvent>> m_fileWriteEvents;
};