#pragma once
#include <iostream>

class Command :public IModule
{
public:
	void Initialize() override
	{
		std::cout << "初期化：コマンドモジュール" << std::endl;
		//std::cout << "ホットリロード" << std::endl;
	}

	void Finalize() override
	{
		std::cout << "解放：コマンドモジュール" << std::endl;
	}

	void Update() override
	{
		//std::cout << "更新：コマンドモジュール" << std::endl;
		
		if (!_flg)
		{
			std::cout << "Helllo World!" << std::endl;
			_flg = true;
		}
		
	}

private:
	bool _flg = false;
};

extern"C" DLLCLIENTAPI IModule* CreateInstance()
{
	return new Command();
}

extern"C" DLLCLIENTAPI void DeleteInstance(IModule*& _ptr)
{
	delete _ptr;
	_ptr = nullptr;
}