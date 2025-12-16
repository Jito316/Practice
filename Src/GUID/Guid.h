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
	
	Guid(const std::string& strGuid)
	{
		FromString(strGuid);
	}

	// V‚µ‚¢GUID‚ğì¬‚·‚é
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
		std::ignore = UuidFromStringA((RPC_CSTR)strGuid.c_str(), &m_guid);
	}

private:
	UUID m_guid = {};
};