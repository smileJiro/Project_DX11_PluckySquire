#pragma once
#include "FatherGame_Progress.h"

BEGIN(Client)
class CMonster;
class CPortalLocker;
class CZetPack_Father;
class CFatherGame_Progress_MakeFather : public CFatherGame_Progress
{
public:
	typedef struct tagFatherGameProgress_MakeFather
	{

	}FATHERGAME_PROGRESS_MAKEFATHER_DESC;

private:
	CFatherGame_Progress_MakeFather(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CFatherGame_Progress_MakeFather() = default;

public:
	virtual HRESULT			Initialize(void* _pArg);
	HRESULT					Progress_Enter() override;		// Active OnEnable 에서 호출.
	void					Progress_Update() override;		
	HRESULT					Progress_Clear() override;		// Clear 조건 검사 + Clear 시 동작 정의
	HRESULT					Progress_Exit() override;		// Active OnDisable 에서 호출.

private: /* ZetPack_Father */
	CZetPack_Father*		m_pZetPack_Father = nullptr;
public:
	static CFatherGame_Progress_MakeFather* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	void					Free() override;
};

END