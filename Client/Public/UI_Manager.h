#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "GameInstance.h"


class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

	enum STAMP { STAMP_STOP, STAMP_BOMB, STAMP_END};

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;




private:
	CGameInstance* m_pGameInstance;
	STAMP		m_eStampIndex = { STAMP_STOP };


public:
	STAMP Get_StampIndex() { return m_eStampIndex; }
	void	Set_StampIndex(STAMP _Stamp) { m_eStampIndex = _Stamp; }



public:
	virtual void Free() override;
};