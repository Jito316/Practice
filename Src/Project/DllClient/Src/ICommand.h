#pragma once

#ifdef DLLCLIENT_EXPORTS
#define DLLCLIENTAPI __declspec(dllexport)
#else
#define DLLCLIENTAPI __declspec(dllimport)
#endif

class DLLCLIENTAPI ICommand
{
public:
	virtual void Call() = 0;
};