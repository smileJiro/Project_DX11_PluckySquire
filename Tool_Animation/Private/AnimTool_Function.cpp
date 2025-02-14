#include "stdafx.h"
#include "AnimTool_Function.h"
#include "Event_Manager.h"
#include "GameObject.h"

namespace AnimTool
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

	void Event_LevelChange(_int _iChangeLevelIndex, _int _iNextChangeLevelIndex)
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

	std::wstring OpenFileDialog(LPCWSTR szFilter)
	{
		TCHAR lpstrFile[MAX_PATH] = L"";// 파일 경로를 저장할 버퍼
		OPENFILENAME ofn;             // 파일 다이얼로그 구조체

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL; // 소유 윈도우 핸들 (NULL로 설정 가능)
		ofn.lpstrFilter = szFilter;
		ofn.lpstrFile = lpstrFile;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		if (GetOpenFileName(&ofn)) {
			return lpstrFile; // 선택된 파일 경로 반환
		}

		return TEXT(""); // 취소 시 빈 문자열 반환
	}

	std::wstring OpenDirectoryDialog()
	{

		CoInitialize(NULL);

		IFileDialog* pfd = NULL;
		HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));

		if (SUCCEEDED(hr)) {
			// 폴더 선택 모드 활성화
			DWORD dwOptions;
			pfd->GetOptions(&dwOptions);
			pfd->SetOptions(dwOptions | FOS_PICKFOLDERS); // 디렉토리 선택 모드

			// 다이얼로그 표시
			hr = pfd->Show(NULL);
			if (SUCCEEDED(hr)) {
				IShellItem* psiResult;
				hr = pfd->GetResult(&psiResult);

				if (SUCCEEDED(hr)) {
					// 디렉토리 경로 가져오기
					PWSTR pszFilePath = NULL;
					psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					// 경로를 std::string으로 변환
					std::wstring ws(pszFilePath);


					// 메모리 해제
					CoTaskMemFree(pszFilePath);
					psiResult->Release();
					pfd->Release();
					CoUninitialize();

					return pszFilePath; // 선택한 디렉토리 경로 반환
				}
			}
			pfd->Release();
		}

		CoUninitialize();
		return TEXT(""); // 취소 시 빈 문자열 반환
	}

	std::wstring SaveFileDialog(LPCWSTR szFilter, const std::wstring& defaultName)
	{
		_tchar filename[MAX_PATH] = L""; // 파일 경로를 저장할 버퍼
		if (!defaultName.empty()) {
			lstrcpyW(filename, defaultName.c_str()); // 기본 파일 이름 설정
		}

		OPENFILENAME ofn; // 파일 대화 상자 구조체 초기화
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL; // 소유 윈도우 핸들 (NULL 가능)
		ofn.lpstrFilter = szFilter; // 필터 설정
		ofn.lpstrFile = filename;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_OVERWRITEPROMPT; // 덮어쓰기 경고 표시

		if (GetSaveFileName(&ofn)) {
			return std::wstring(filename); // 선택된 파일 경로 반환
		}

		return L""; // 취소 시 빈 문자열 반환
	}

	std::string Get_SuperKey(const filesystem::path& _path)
	{
		filesystem::path strPath = _path;
		string strSuperKey = strPath.parent_path().filename().string();
		while (strSuperKey._Equal("Frames" ) || strSuperKey._Equal("Textures") || strSuperKey._Equal("Sprites"))
		{
			strPath = strPath.parent_path();
			strSuperKey = strPath.parent_path().filename().string();
		}
		return strSuperKey;
	}

	std::string Get_TextureName_FromKey(string _strKey)
	{
		size_t iPos = _strKey.find("$");
		if (iPos != string::npos)
			return _strKey.substr(iPos + 1);
		return "";
	}

	std::string MakeTextureKey(const filesystem::path& _path)
	{
		string wstrKey = Get_SuperKey(_path) + "$";
		wstrKey  += _path.filename().replace_extension().string();
		return wstrKey;
	}



	F_DIRECTION To_FDirection(_vector _vDir)
	{
		//가로축이 더 클 때
		if (abs(_vDir.m128_f32[0]) > abs(_vDir.m128_f32[1]))
			return _vDir.m128_f32[0] < 0 ? F_DIRECTION::LEFT : F_DIRECTION::RIGHT;
		//세로축이 더 클 때
		else
			return _vDir.m128_f32[1] > 0 ? F_DIRECTION::UP : F_DIRECTION::DOWN;

		return F_DIRECTION::F_DIR_LAST;
	}
}