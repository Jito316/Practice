#pragma once

#include <string>

#pragma comment(lib ,"rpcrt4.lib")
#include <rpc.h>

class Guid
{
public:

	Guid()
	{
		NewGuid();
	}

	// êVÇµÇ¢GUIDÇçÏê¨Ç∑ÇÈ
	void NewGuid()
	{
		if (UuidCreate(&m_guid) != RPC_S_OK)
		{

		}
	}

	std::string ToString() const
	{
		std::string ret;
		RPC_CSTR String;
		if (UuidToStringA(&m_guid, &String) == RPC_S_OK)
		{
			ret = (char*)String;
		}
		return ret;
	}

	void FromString(const std::string& strGuid)
	{
		if (UuidFromStringA((RPC_CSTR)strGuid.c_str(), &m_guid) != RPC_S_OK) 
		{
		
		}
	}

private:
	UUID m_guid = {};
};