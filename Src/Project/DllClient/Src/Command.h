#pragma once
#include <iostream>

class Command :public ICommand
{
	void Call()override 
	{
		std::cout << "Hello World!!" << std::endl;
	}
};

DLLCLIENTAPI ICommand* CreateInstance()
{
	return new Command();
}

DLLCLIENTAPI void DeleteInstance(ICommand*& _ptr)
{
	delete _ptr;
	_ptr = nullptr;
}