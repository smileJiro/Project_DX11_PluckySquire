#include "stdafx.h"
#include "Map_Tool_Function.h"
#include "Event_Manager.h"
#include "GameObject.h"
#include <commdlg.h>

/* 함수 구현부 */
namespace Map_Tool
{

	void Event_CreateObject(_uint _iCurLevelIndex, const _tchar* _strLayerTag, Engine::CGameObject* _pGameObject)
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::CREATE_OBJECT;
		tEvent.Parameters.resize(3); // NumParameters

		tEvent.Parameters[0] = (DWORD_PTR)_iCurLevelIndex;
		tEvent.Parameters[1] = (DWORD_PTR)_strLayerTag;
		tEvent.Parameters[2] = (DWORD_PTR)_pGameObject;

		CEvent_Manager::GetInstance()->AddEvent(tEvent);
	}


	void Event_DeleteObject(Engine::CGameObject* _pGameObject)
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::DELETE_OBJECT;
		tEvent.Parameters.resize(1); // NumParameters

		tEvent.Parameters[0] = (DWORD_PTR)_pGameObject;

		CEvent_Manager::GetInstance()->AddEvent(tEvent);
	}

	void Event_LevelChange(_uint _iChangeLevelIndex, _uint _iNextChangeLevelIndex)
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::LEVEL_CHANGE;
		tEvent.Parameters.resize(2); // NumParameters

		tEvent.Parameters[0] = (DWORD_PTR)_iChangeLevelIndex;
		tEvent.Parameters[1] = (DWORD_PTR)_iNextChangeLevelIndex;

		CEvent_Manager::GetInstance()->AddEvent(tEvent);
	}

	void Event_SetActive(CBase* _pObject, _bool _isActive, _bool _isDelay)
	{
		EVENT tEvent;
		tEvent.eType = EVENT_TYPE::SET_ACTIVE;
		tEvent.Parameters.resize(3); // NumParameters

		tEvent.Parameters[0] = (DWORD_PTR)_pObject;
		tEvent.Parameters[1] = (DWORD_PTR)_isActive;
		tEvent.Parameters[2] = (DWORD_PTR)_isDelay;

		CEvent_Manager::GetInstance()->AddEvent(tEvent);
	}


	std::wstring StringToWstring(const std::string& _str)
	{
		if (_str.empty()) return L"";

		int iWstrLen = MultiByteToWideChar(CP_UTF8, 0, _str.c_str(), -1, nullptr, 0);
		if (iWstrLen <= 0) return L"";

		std::wstring wstr(iWstrLen, 0);
		MultiByteToWideChar(CP_UTF8, 0, _str.c_str(), -1, &wstr[0], iWstrLen);

		// Null-terminated string에서 null 제거
		wstr.resize(iWstrLen - 1);
		return wstr;

		/* Ver.14 */
		// std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		//return converter.from_bytes(_strUTF8);
	}

	std::string WstringToString(const std::wstring& _strWide)
	{
		if (_strWide.empty()) return "";

		int iUTF8Len = WideCharToMultiByte(CP_UTF8, 0, _strWide.c_str(), -1, nullptr, 0, nullptr, nullptr);
		if (iUTF8Len <= 0) return "";

		std::string strUTF8(iUTF8Len, 0);
		WideCharToMultiByte(CP_UTF8, 0, _strWide.c_str(), -1, &strUTF8[0], iUTF8Len, nullptr, nullptr);

		// Null-terminated string에서 null 제거
		strUTF8.resize(iUTF8Len - 1);
		return strUTF8;

		/* Ver.14 */
		//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		//return converter.to_bytes(_strWide);
	}


	pair<_wstring, _wstring> Get_FileName_From_Path(const _wstring& filePath)
	{
		size_t iPathPosIndex = filePath.rfind(L'\\');
		pair<_wstring, _wstring> pair = { L"",L"" };

		if (iPathPosIndex == _wstring::npos)
		{
			iPathPosIndex = filePath.rfind(L'/');
		}

		_wstring fileName = (iPathPosIndex != _wstring::npos) ? filePath.substr(iPathPosIndex + 1) : filePath;

		size_t iDotPos = fileName.rfind(L'.');
		if (iDotPos != _wstring::npos)
		{
			pair.first = fileName.substr(0, iDotPos);
			pair.second = fileName.substr(iDotPos + 1);
		}

		return pair; // 확장자가 없으면 빈 문자열 반환
	}

	_wstring	Get_Path_From_FullPath(const _wstring& filePath)
	{
		size_t iPathPosIndex = filePath.rfind(L'\\');
		if (iPathPosIndex == _wstring::npos)
		{
			iPathPosIndex = filePath.rfind(L'/');
		}

		return (iPathPosIndex != _wstring::npos) ? filePath.substr(0, iPathPosIndex) : L"";
	}


	_bool Path_String_Validation_Check(const _string _strText)
	{
		if (_strText.empty() ||
			_strText.rfind(".") != string::npos ||
			_strText.rfind(":") != string::npos ||
			_strText.rfind("*") != string::npos ||
			_strText.rfind("?") != string::npos ||
			_strText.rfind("\"") != string::npos ||
			_strText.rfind("<") != string::npos ||
			_strText.rfind(">") != string::npos ||
			_strText.rfind("|") != string::npos ||
			_strText.rfind("/") != string::npos ||
			_strText.rfind("\\") != string::npos
			)
			return false;
		return true;
	}
	_bool Path_String_Validation_Check(const _wstring _strText)
	{
		return Path_String_Validation_Check(WstringToString(_strText));
	}

	_bool ContainString(const _string _strSourceText, const _string _strDestText)
	{
		return string::npos != _strSourceText.find(_strDestText);
	}
	
	_bool ContainWstring(const _wstring _strSourceText, const _wstring _strDestText)
	{
		return string::npos != _strSourceText.find(_strDestText);
	}

	_bool EndString(const _string _strSourceText, const _string _strDestText)
	{
		return _strSourceText.rfind(_strDestText) == _strSourceText.size() - _strDestText.size();

	}
	_string	OutName(const _string _strName)
	{
		return _strName.substr(_strName.find("'") + 1, _strName.size() - _strName.find("'") - 2);
	}


	_bool Open_Dialog(const _wstring _strDefaultDirectory, const _wstring _strFilter, wstring& _strReturnDirectory)
	{
		_tchar originalDir[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, originalDir);

		OPENFILENAME ofn = {};
		_tchar szName[MAX_PATH] = {};
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = g_hWnd;
		ofn.lpstrFile = szName;
		ofn.nMaxFile = sizeof(szName);
		ofn.lpstrFilter = _strFilter.c_str();
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = nullptr;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = _strDefaultDirectory.c_str();
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		if (GetOpenFileName(&ofn))
		{
			SetCurrentDirectory(originalDir);
			_strReturnDirectory = szName;
			return true;
		}

		return false;
	}

}


