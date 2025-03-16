#pragma once
#include "Base.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CFormation;
class CMonster;

class CFormation_Manager : public CBase
{
	DECLARE_SINGLETON(CFormation_Manager)
public:
	enum Formation_PatrolPoint
	{
		FORMATION1,
		FORMATION2,
		LAST
	};

private:
	CFormation_Manager();
	virtual ~CFormation_Manager() = default;

public:
	HRESULT	Initialize();
	void	Update(_float _fTimeDelta);

public:
	void Initialize_Formation_PatrolPoints(_uint _iIndex, CFormation* _pFormation);
	void Register_Formation(CFormation* _pFormation);

	//비어있는 위치로 추가
	_bool Add_To_Formation(CMonster* _pMember, CFormation** _pFormation);

private:
	HRESULT Ready_Chapter8_Formation();

private:
	class CGameInstance*	m_pGameInstance = { nullptr };

	vector<CFormation*> m_Formations;
	_uint m_iNumFormations = { 0 };

public:
	static CFormation_Manager* Create();
	virtual void Free() override;
};

END