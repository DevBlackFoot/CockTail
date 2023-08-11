#pragma once
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <string>
#include <tchar.h>

namespace GraphicsEngineSpace
{
	/**
	 * \brief ���ڿ� ���ۿ� ������ �ִ� �Լ��� �����ϴ� Ŭ�����Դϴ�. By.DUOL Common
	 */
	class StringHelper
	{
	public:
		inline static std::string WStringToString(const wchar_t* target)
		{
			const int stringLength = static_cast<int>(std::wcslen(target));

			// WCHAR �� �� ���� �� 2����Ʈ�� ����.
			const int bufferLength = stringLength * 2 + 1;

			char* buffer = new char[bufferLength];

			// WideCharToMultiByte �Լ��� ���ڿ��� ���� �ڵ����� null ���� ('\0')�� �־����� �ʽ��ϴ�.
			// ���� ���ڿ��� ��ȯ�� ��ģ �� �� �ڿ��� �������� null���ڸ� �־��־�� �մϴ�.
			int end = WideCharToMultiByte(CP_UTF8, 0, target, stringLength,
				buffer, bufferLength,
				nullptr, nullptr);

			buffer[end] = '\0';

			auto retVal = std::string{ buffer };

			delete[] buffer;

			return retVal;
		}

		inline static std::string WStringToString(const std::wstring& target)
		{
			return WStringToString(target.c_str());
		}

		inline static std::string ToString(const std::wstring& target)
		{
			return WStringToString(target);
		}

		inline static std::wstring StringToWString(const char* target)
		{
			const int stringLength = static_cast<int>(strlen(target));

			const int bufferLength = stringLength + 2;

			wchar_t* buffer = new wchar_t[bufferLength];

			int end = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, target, stringLength,
				buffer, bufferLength);

			buffer[end] = '\0';

			auto retVal = std::wstring{ buffer };

			delete[] buffer;

			return retVal;
		}

		inline static std::wstring StringToWString(const std::string& target)
		{
			return StringToWString(target.c_str());
		}

		inline static std::wstring ToWString(const std::string& target)
		{
			return StringToWString(target);
		}

		inline static std::wstring WStringToUTFWString(const wchar_t* target)
		{
			auto retValString = WStringToString(target);

			std::wstring retVal;
			retVal.assign(retValString.begin(), retValString.end());

			return retVal;
		}

		inline static std::wstring WStringToUTFWString(const std::wstring& target)
		{
			return WStringToUTFWString(target.c_str());
		}

	};
}