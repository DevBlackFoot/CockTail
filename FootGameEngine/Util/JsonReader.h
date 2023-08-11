#pragma once

/**
* \brief Json을 읽어오는 클래스
*
* rapidjson을 wrapping 한 클래스이다.
* 좀 더 쉽게 쓰고 알아보기 좋으려고 했습니다.
*
* 2023.01.19(목) Dev.Newbie
*/
namespace GameEngineSpace
{
	using TValue = rapidjson::GenericValue<rapidjson::UTF16<TCHAR>>;

	class JsonObject
	{
	private:
		TValue& json_obj;

	public:
		JsonObject(TValue& json_obj)
			: json_obj(json_obj) {}
		
		std::wstring GetString(const std::wstring& key) const 
		{
			if (json_obj.HasMember(key.c_str()) && json_obj[key.c_str()].IsString())
				return json_obj[key.c_str()].GetString();
			else
			{
				MessageBox(0, TEXT("Json GetString() Failed"), 0, 0);
				return L"";
			}
		}

		int GetInt(const std::wstring& key) const 
		{
			if (json_obj.HasMember(key.c_str()) && json_obj[key.c_str()].IsInt())
				return json_obj[key.c_str()].GetInt();
			else
			{
				MessageBox(0, TEXT("Json GetInt() Failed."), 0, 0);
				return 0;
			}
		}

		float GetFloat(const std::wstring& key) const 
		{
			if (json_obj.HasMember(key.c_str()) && json_obj[key.c_str()].IsFloat())
				return json_obj[key.c_str()].GetFloat();
			else
			{
				MessageBox(0, TEXT("Json GetFloat() Failed."), 0, 0);
				return 0.f;
			}
		}

		bool GetBool(const std::wstring& key) const 
		{
			if (json_obj.HasMember(key.c_str()) && json_obj[key.c_str()].IsBool())
				return json_obj[key.c_str()].GetBool();
			else
			{
				MessageBox(0, TEXT("Json GetBool() Failed."), 0, 0);
				return false;
			}
		}

	

		template <typename T>
		std::vector<T> GetArray(const std::wstring& key);
	};

	using JsonUnicode = rapidjson::UTF16<TCHAR>;
	using TDocument = rapidjson::GenericDocument<JsonUnicode>;

	class JsonReader
	{
	private:
		std::unique_ptr<TDocument> doc;

	public:
		JsonReader(const std::string& json_path);
		~JsonReader();

	public:
		std::wstring GetString(const std::wstring& key);

		int GetInt(const std::wstring& key);

		float GetFloat(const std::wstring& key);

		bool GetBool(const std::wstring& key);

		template <typename T>
		std::vector<T> GetArray(const std::wstring& key);

		GameEngineSpace::JsonObject GetJsonObject(const std::wstring& key);

		std::vector<GameEngineSpace::JsonObject> GetJsonObjects();
	};
		
	template<typename T>
	inline std::vector<T> JsonReader::GetArray(const std::wstring& key)
	{
		auto it = doc->FindMember(key.c_str());

		std::vector<T> tmpArray;

		if (it != doc->MemberEnd() && it->value.IsArray())
		{
			for (rapidjson::SizeType i = 0; i < it->value.Size(); i++)
			{
				if (it->value[i].Is<T>())
					tmpArray.push_back(it->value[i].Get<T>());
				else
					MessageBox(0, TEXT("Array element type does not match"), 0, 0);
			}

			return tmpArray;
		}
		else
		{
			MessageBox(0, TEXT("Json GetArray() Failed."), 0, 0);
		}

		return tmpArray;
	}

	template<typename T>
	inline std::vector<T> JsonObject::GetArray(const std::wstring& key)
	{
		auto it = json_obj.FindMember(key.c_str());

		std::vector<T> tmpArray;

		if (it != json_obj.MemberEnd() && it->value.IsArray())
		{
			for (rapidjson::SizeType i = 0; i < it->value.Size(); i++)
			{
				if (it->value[i].Is<T>())
					tmpArray.push_back(it->value[i].Get<T>());
				else
					MessageBox(0, TEXT("Array element type does not match"), 0, 0);
			}

			return tmpArray;
		}
		else
		{
			MessageBox(0, TEXT("Json GetArray() Failed."), 0, 0);
		}

		return tmpArray;
	}
}

