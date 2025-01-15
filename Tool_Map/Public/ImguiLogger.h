#pragma once


#include "Base.h"
#include "Map_Tool_Defines.h"

BEGIN(Engine)
class CGameInstance;
END


BEGIN(Map_Tool)

class CImguiLogger final : public CBase
{
public :
	typedef struct tagLogMsg
	{
		string strLogMsg;
		LOG_TYPE eType;
	} LOG_MSG;

private :

	typedef struct ImguiLog
	{
		ImGuiTextBuffer     Buf;
		bool                AutoScroll;

		ImguiLog()
		{
			AutoScroll = true;
			Clear();
		}

		void    Clear() { Buf.clear(); 
		}

		void    AddLog(const _char* _fmt, ...) IM_FMTARGS(2)
		{
			va_list args;
			va_start(args, _fmt);
			Buf.appendfv(_fmt, args);
			va_end(args);
		}

		void    Draw(const _char* _title, _bool* _p_opened = NULL)
		{

			ImGui::Begin(_title, _p_opened);
			if (ImGui::Button("Clear"))
				Clear();
			ImGui::SameLine();
			ImGui::Checkbox("Auto scroll", &AutoScroll);

			ImGui::Separator();
			if (ImGui::BeginChild("scrolling", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar))

			ImGui::TextUnformatted(Buf.begin());
			if (AutoScroll && ImGui::GetScrollY() < ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);
			ImGui::EndChild();
			ImGui::End();


		}
	}LOGOBJ;

private :
	explicit CImguiLogger();
	~CImguiLogger() = default;

	

public :
	void Add_Log(const string& _strLog, LOG_TYPE _eType);
	void Add_Log(const wstring& _strLog, LOG_TYPE _eType);
	void Clear_Log() { m_vecLog.clear(); }
	
	HRESULT Draw_Log();

	LOG_MSG* Get_BackMsg() { if (m_vecLog.empty()) return nullptr; return &m_vecLog.back(); }

	void Set_Lock(_bool _bLock) { m_bLock = _bLock; }

private :
	CGameInstance*	m_pGameInstance = nullptr;
	LOGOBJ			m_tLog;
	vector<LOG_MSG>	m_vecLog;
	bool			m_bAutoScroll = { false };
	bool			m_bLock = false;
	string			m_arrTypeText[LOG_END]
		=
	{
		"",
		"[LOADING] ",
		"[ERROR] ",
		"[SAVE] ",
		"[BINARY] ",
		"[LOAD] ",
		"[CREATE] ",
		"[DELETE] ",
	};

public :
	virtual void Free();
	static CImguiLogger* Create();
};

END