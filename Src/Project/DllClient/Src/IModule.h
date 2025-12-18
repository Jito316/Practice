#pragma once

#ifdef DLLCLIENT_EXPORTS
#define DLLCLIENTAPI __declspec(dllexport)
#else
#define DLLCLIENTAPI __declspec(dllimport)
#endif

class DLLCLIENTAPI IModule
{
public:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Finalize() = 0;
};