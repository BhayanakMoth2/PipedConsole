#pragma once
#include <Windows.h>
#include <exception>
#include <string>
#include <locale>
#include <codecvt>
namespace ErrorHandling
{
	class WinError
	{
		std::string m_Message = "None";
	public:
		WinError(DWORD p_GLECode, std::string p_Message)
		{
			char buffer[256];
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, p_GLECode, NULL, (LPSTR)buffer, 255, NULL);
			m_Message = std::string(buffer);
			m_Message = p_Message + " " + m_Message;
		}
		std::string what() const
		{
			return m_Message;
		}

	};
}
