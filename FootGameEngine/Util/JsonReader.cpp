#include "GamePch.h"
#include "JsonReader.h"
#include <fstream>

namespace GameEngineSpace
{
	JsonReader::JsonReader(const std::string& json_path)
	{
		doc = std::make_unique<TDocument>();

		FILE* fp = nullptr;

		std::wstring path(json_path.begin(), json_path.end());

		errno_t err = _wfopen_s(&fp, path.c_str(), _T("rb"));
		//errno_t err = fopen_s(&fp, path.c_str(), ("rb"));
		
		char readBuffer[23768];

		rapidjson::FileReadStream readStream{ fp, readBuffer, sizeof(readBuffer) };
		rapidjson::AutoUTFInputStream<unsigned, rapidjson::FileReadStream> eis(readStream);  // wraps bis into eis
		doc->ParseStream<0, rapidjson::AutoUTF<unsigned>>(eis);

		/*std::ifstream file(json_path);

		std::stringstream read_Data;

		read_Data << file.rdbuf();

		std::string read_StringData = read_Data.str();

		int i = 0;

		while (read_StringData[i] != '[')
			i++;

		read_StringData = read_StringData.substr(i);

		doc->Parse(read_StringData.c_str());*/

		/*if(!file.is_open())
			MessageBox(0, TEXT("Json Open Failed"), 0, 0);

		std::string json_str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());*/
		
		if (doc->HasParseError())
			MessageBox(0, TEXT("Json ParseError"), 0, 0);
		
		fclose(fp);

		//file.close();
	}

	JsonReader::~JsonReader()
	{}

	std::wstring JsonReader::GetString(const std::wstring& key)
	{
		auto it = doc->FindMember(key.c_str());

		if (it != doc->MemberEnd() && it->value.IsString())
		{
			return it->value.GetString();
		}
		else 
		{
			MessageBox(0, TEXT("Json GetString() Failed"), 0, 0);
		
			return L"";
		}
	}

	int JsonReader::GetInt(const std::wstring& key)
	{
		auto it = doc->FindMember(key.c_str());
		
		if (it != doc->MemberEnd() && it->value.IsInt())
		{
			return it->value.GetInt();
		}
		else
		{
			MessageBox(0, TEXT("Json GetInt() Failed."), 0, 0);
			
			return 0;
		}
	}

	float JsonReader::GetFloat(const std::wstring& key)
	{
		auto it = doc->FindMember(key.c_str());

		if (it != doc->MemberEnd() && it->value.IsFloat())
		{
			return it->value.GetFloat();
		}
		else
		{
			MessageBox(0, TEXT("Json GetFloat() Failed."), 0, 0);

			return 0.f;
		}
	}

	bool JsonReader::GetBool(const std::wstring& key)
	{
		auto it = doc->FindMember(key.c_str());

		if (it != doc->MemberEnd() && it->value.IsBool())
		{
			return it->value.GetBool();
		}
		else
		{
			MessageBox(0, TEXT("Json GetBool() Failed."), 0, 0);

			return false;
		}
	}

	GameEngineSpace::JsonObject JsonReader::GetJsonObject(const std::wstring& key)
	{
		if (doc->HasMember(key.c_str()) && doc->FindMember(key.c_str())->value.IsObject())
			return JsonObject(doc->FindMember(key.c_str())->value);
		else
		{
			MessageBox(0, TEXT("Json GetJsonObject Failed"), 0, 0);
		}
	}

	std::vector<GameEngineSpace::JsonObject> JsonReader::GetJsonObjects()
	{
		std::vector<GameEngineSpace::JsonObject> jsonObjects;

		for (auto it = doc->Begin(); it != doc->End(); ++it)
		{
			GameEngineSpace::JsonObject jsonObject(*it);

			jsonObjects.push_back(jsonObject);
		}

		return jsonObjects;
	}
}