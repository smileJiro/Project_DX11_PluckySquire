#pragma once
#include "Character.h"
BEGIN(Engine)
class CCollider_Circle;
class CModelObject;
END

BEGIN(Client)
class CDefenderPerson :
    public CCharacter
{
public:
	enum FX_ANIM_STATE
	{
		APPEAR,
		BEACON,
		DISAPPEAR,
	};
	enum FOLLOW_STATE
	{
		STOP,
		FOLLOW,
	};
	enum PART_ID
	{
		PERSON_PART_BODY,
		PERSON_PART_FX,
		PERSON_PART_SHIELD,
		PERSON_PART_LAST
	};
protected:
	CDefenderPerson(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CDefenderPerson(const CDefenderPerson& _Prototype);
	virtual ~CDefenderPerson() = default;
public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Update(_float _fTimeDelta) override;

private:
	HRESULT	Ready_PartObjects();
	HRESULT	Ready_Components();
public:
	virtual void On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void Enter_Section(const _wstring _strIncludeSectionName) override;

public:
	void Start_FollowObject(CGameObject* _pObject);
	void Dissapear();

	void On_BodyAnimEnd(COORDINATE _eCoord, _uint iAnimIdx);
	void On_FXAnimEnd(COORDINATE _eCoord, _uint iAnimIdx);
	_vector Get_ScrolledPosition(_vector _vPosition);
private:
	class CSection_Manager* m_pSectionManager = nullptr;
	_float m_fMoveSpeed = 500.f;
	CCollider_Circle* m_pBodyCollider = nullptr;
	CGameObject* m_pFollowObject = nullptr;
	_float m_fFollowDistance = 70.f;
	_float m_fArrivalDistance = 30.f;

	FOLLOW_STATE m_eFollowState = STOP;

	CModelObject* m_pBody = nullptr;
	CModelObject* m_pFX = nullptr;
	CModelObject* m_pShield = nullptr;
public:
	static CDefenderPerson* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END