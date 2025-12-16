#include "AssetManager.h"
#include <iostream>
#include "Application/Application.h"

// Metaファイルを探してAddressablesListを作成する
void AssetManager::CreateAddressablesList()
{
	std::ofstream log(_logFileName);

	for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(_assetFilePass))
	{
		// メタファイルかどうか
		if (entry.is_regular_file() && entry.path().extension().string() == _metaFileExtentionName)
		{
			std::ifstream meta(entry.path().string());
			nlohmann::json j;
			meta >> j;

			// AddressableName
			std::string addressable_string = j["AddressableName"];
			Guid addressable(addressable_string);

			// 今回実行時のファイルパスの作成
			std::string filePath = j["SourceFile"];

			std::filesystem::path onlydi = entry.path();
			onlydi.remove_filename();
			std::string directory = onlydi.relative_path().string();
			filePath = directory + filePath;

			// メタデータから参照ファイル情報の作成
			MetaData metaData;
			metaData.filePath = filePath;
			metaData.guid = addressable;
			metaData.writeTime  = std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(filePath).time_since_epoch()).count();

			// Addressableの被りは許さず
			if (_addressables.find(addressable_string) != _addressables.end())
			{
				log << "error! : " << addressable_string << " This AddressableName is Conflict!" << " filePath : " << metaData.filePath << std::endl;
				assert(0 && "AddressableNameが被っています！！ Logファイルを参照して下さい");
			}

			// AddressableNameをキーにしてデータを覚えておく
			_addressables[addressable_string] = metaData;
		}
	}
}

// AddressableNameからファイルパス取得
std::string AssetManager::GetFilePathFromAddressableName(const std::string& addressableName)
{
	auto it = _addressables.find(addressableName);
	if (it == _addressables.end())
	{
		assert(0 && "指定されたAddressableNameが見つかりません！");
	}

	return _addressables[addressableName].filePath;
}

// Assetフォルダ以下の対応ファイル全てにMetaファイルを作っていく
void AssetManager::CreateMetaFileForAllFiles()
{
	// log出力先
	std::ofstream log(_logFileName);

	// 対応するAssetの拡張子を登録→最終的には外部ファイルに吐き出す
	_supportedExtensions.clear();
	_supportedExtensions.push_back(".txt");
	// _supportedExtensions.push_back(".gltf");
	// _supportedExtensions.push_back(".png");
	// …more

	// 指定されたアセットフォルダ以下をクロール
	for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(_assetFilePass))
	{
		// AssetManagerがサポートしているファイルだった
		if (entry.is_regular_file() && IsSupportedFile(entry.path()))
		{
			// メタファイルが有るか
			std::filesystem::path metafilePath = entry.path();
			metafilePath.replace_filename(entry.path().filename().string() + _metaFileExtentionName);
			if (std::filesystem::exists(metafilePath) == false)
			{
				// メタファイルがなかったら新規作成
				std::ofstream metaFile(metafilePath);
				log << "CreateMetaFile!  " << metafilePath << std::endl;

				// メタファイルに書き込むデータの作成
				metaFile << CreateMetaFileForFile(entry.path());
			}
		}
	}
}

// ファイル拡張子がサポートしている形式か調べる
bool AssetManager::IsSupportedFile(const std::filesystem::path& filePath)
{
	for (auto& ext : _supportedExtensions)
	{
		if (filePath.extension().string() == ext) { return true; }
	}
	return false;
}

void AssetManager::FileUpdate()
{
	bool isPress = false;
	while (!Application::Instance().IsEnd())
	{
		if (GetAsyncKeyState('U') & 0x8000)
		{
			if (isPress)continue;
			isPress = true;
		}
		else 
		{
			isPress = false;
			continue;
		}

		std::cout << "ファイル監視" << std::endl;
		for (auto& [id, data] : _addressables)
		{
			std::filesystem::file_time_type tp = std::filesystem::last_write_time(data.filePath);
			auto writeTime = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();

			if (data.writeTime != writeTime) 
			{
				data.writeTime = writeTime;
				for (auto& it : m_fileWriteEvents) 
				{
					it->OnWrite(data);
				}
			}
		}
	}
}

// 渡されたファイルに対してMetaファイルを作成する
nlohmann::json AssetManager::CreateMetaFileForFile(const std::filesystem::path& srcFile)
{
	nlohmann::json j;
	j["SourceFile"] = srcFile.filename().string();
	j["AddressableName"] = Guid().ToString();
	return j;
}

// Metaファイル残削除関数、めっちゃ危険
void AssetManager::DeleteAllMetaFiles()
{
	// Log保存場所
	std::ofstream log(_logFileName);

	// Assetフォルダをクロール
	for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(_assetFilePass))
	{
		// Metaファイルじゃなかったら無視
		if (entry.path().extension().string() != _metaFileExtentionName) { continue; }

		// 削除
		if (std::filesystem::remove(entry.path()))
		{
			log << "Delete Mata File! " << entry.path().string() << std::endl;
		}
	}
}