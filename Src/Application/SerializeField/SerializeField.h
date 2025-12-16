#pragma once
class IGameObject;
class ITransform;

struct BaseSerializeField
{
	virtual void ToJson(nlohmann::json& _data, std::string_view _key) = 0;
	virtual void ToValue(const nlohmann::json& _data, std::string_view _key) = 0;
	void* instance = 0;
};

template<class T>
struct SerializeField : BaseSerializeField
{
	SerializeField()
	{
		instance = (void*)new T;
	}

	SerializeField(const T& _value)
	{
		auto temp = new T;
		(*temp) = _value;
		instance = (void*)temp;
	}

	~SerializeField()
	{
		delete (T*)instance;
	}

	T& value() { return *(T*)instance; }
	const T& value() const { return *(T*)instance; }

	// *** 演算子オーバーロード ***
	operator T& () { return value(); }
	operator const T& () const { return value(); }

	T* operator->() { return &value(); }
	const T* operator->() const { return &value(); }

	void ToJson(nlohmann::json& _data, std::string_view _key) override
	{
		JsonHelper::ToJson(_data, _key.data(), value());
	}

	void ToValue(const nlohmann::json& _data, std::string_view _key) override
	{
		JsonHelper::ToValue(_data, _key.data(), value());
	}
};

inline void AllSerializeField_ToJson(BaseSerializeField* _start, BaseSerializeField* _end, nlohmann::json& _data, std::string_view _key)
{
	int num = 0;
	while (_start != _end)
	{
		_start->ToJson(_data[_key.data()], std::to_string(++num).data());
		_start++;
	}
	_start->ToJson(_data[_key.data()], std::to_string(num).data());
}
inline void AllSerializeField_ToJson(void* _start, int _number, nlohmann::json& _data, std::string_view _key)
{
	BaseSerializeField* start = (BaseSerializeField*)_start;

	int num = 0;
	for (int i = 0; i < _number; ++i)
	{
		start->ToJson(_data[_key.data()], std::to_string(++num).data());
		start++;
	}
}
inline void AllSerializeField_ToValue(BaseSerializeField* _start, BaseSerializeField* _end, const nlohmann::json& _data, std::string_view _key)
{
	if (!_data.contains(_key.data()))return;

	int num = 0;
	while (_start != _end)
	{
		_start->ToValue(_data[_key.data()], std::to_string(++num).data());
		_start++;
	}
	_start->ToValue(_data[_key.data()], std::to_string(num).data());
}
inline void AllSerializeField_ToValue(BaseSerializeField* _start, BaseSerializeField* _end, const nlohmann::json& _data, std::string_view _key)
{
	if (!_data.contains(_key.data()))return;

	int num = 0;
	while (_start != _end)
	{
		_start->ToValue(_data[_key.data()], std::to_string(++num).data());
		_start++;
	}
	_start->ToValue(_data[_key.data()], std::to_string(num).data());
}

class Obejct
{
	SerializeField<std::string> m_uuid;
};

class GameObejct : public Obejct
{
	SerializeField<std::string> m_name;
};
