#pragma once
#include "Base.h"
/* 1. 각각의 Progress에 Enter, Update, Clear, Exit을 정의한다. */
/* 2. Progress의 현재 실행 상태 체크는 Active를 통해 처리. */
/* 3. Clear 여부를 체크 */

BEGIN(Engine)
class CGameInstance;
END
BEGIN(Client)
class CFatherGame_Progress abstract : public CBase
{
public:
	typedef struct tagFatherGameProgress
	{

	}FATHERGAME_PROGRESS_DESC;

protected:
	CFatherGame_Progress(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CFatherGame_Progress() = default;

public:
	virtual HRESULT			Initialize(void* _pArg);
	virtual HRESULT			Progress_Enter() = 0;
	virtual void			Progress_Update() = 0;
	virtual HRESULT			Progress_Clear() = 0;
	virtual HRESULT			Progress_Exit() = 0;


public:
	// Get
	_bool					Is_Clear() const { return m_isClearProgress; }

	// Set

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	CGameInstance*			m_pGameInstance = nullptr;

protected:
	_bool					m_isClearProgress = false;

private:
	void					Active_OnEnable() override;
	void					Active_OnDisable() override;

public:
	void Free() override;
};
END
