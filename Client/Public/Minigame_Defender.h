#pragma once
#include "ModelObject.h"
#include "DefenderMonster.h"
#include "DefenderSpawner.h"
BEGIN(Client)
class CDefenderSpawner;
class CDefenderPlayer;

class CMiniGame_Defender :
    public CModelObject
{
public:
	typedef struct tagDefender_ControllTowerDesc : public CModelObject::MODELOBJECT_DESC
	{

	}DEFENDER_CONTROLLTOWER_DESC;
private:
	explicit CMiniGame_Defender(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CMiniGame_Defender(const CMiniGame_Defender& _Prototype);
	virtual ~CMiniGame_Defender() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;

private:
	HRESULT Ready_Spanwer();
	virtual void Enter_Section(const _wstring _strIncludeSectionName)override;
private:
	CDefenderPlayer* m_pDefenderPlayer = nullptr;
	_bool m_bGameStart = false;

	map<DEFENDER_MONSTER_ID, wstring> m_mapMonsterPrototypeTag;
	map< DEFENDER_MONSTER_ID, CDefenderSpawner*> m_Spawners;


	_float m_fCenterHeight = 0.f;
	_float m_fFloorHeight = -350.f;
	_float m_fSpawnDistance = 1000.f;//플레이어와의 거리
	_float m_fTimeAcc = 0.f;
public:
	static CMiniGame_Defender* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END