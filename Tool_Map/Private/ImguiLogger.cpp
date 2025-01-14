#include "stdafx.h"
#include "ImguiLogger.h"
#include "GameInstance.h"



CImguiLogger::CImguiLogger()
{
}

void CImguiLogger::Add_Log(const string& strLog, LOG_TYPE eType)
{
	if (m_bLock) return;
	LOG_MSG logmsg = { strLog + "\n", eType };
	m_tLog.AddLog((("%s"+ logmsg.strLogMsg).c_str()), m_arrTypeText[logmsg.eType].c_str());

}

void CImguiLogger::Add_Log(const wstring& strLog, LOG_TYPE eType)
{
	
	string TagText = m_pGameInstance->WStringToString(strLog);
	Add_Log(TagText, eType);
}

HRESULT CImguiLogger::Draw_Log()
{
	//ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
	//m_tLog.Clear();
	//string strLogTypeText;
	//for (const LOG_MSG& Msg : m_vecLog)
	//	m_tLog.AddLog((("%s"+Msg.strLogMsg).c_str()), m_arrTypeText[Msg.eType].c_str());

	m_tLog.Draw("Log");

    return S_OK;
}

void CImguiLogger::Free()
{
	__super::Free();
}

CImguiLogger* CImguiLogger::Create()
{
    return new CImguiLogger();
}
