#pragma once
#include "PartObject.h"

BEGIN(Client)
class CMonster;
class CButterGrump_RightEye : public CPartObject
{
public:
	typedef struct tagButterGrump_RightEyeDesc : public CPartObject::PARTOBJECT_DESC
	{
		class CMonster* pParent = nullptr;
	}BUTTERGRUMP_RIGHTEYE_DESC;

private:
	CButterGrump_RightEye(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CButterGrump_RightEye(const CButterGrump_RightEye& _Prototype);
	virtual ~CButterGrump_RightEye() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void	Update(_float _fTimeDelta) override;
	virtual void	Late_Update(_float _fTimeDelta) override;
	virtual HRESULT	Render() override;


	virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other) override;
	virtual void OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void Active_OnEnable() override;
	virtual void Active_OnDisable() override;

private:
	virtual	HRESULT					Ready_ActorDesc(void* _pArg);

private:
	_bool m_isHitEnable = { true };
	CMonster* m_pParent = { nullptr };

public:
	static CButterGrump_RightEye* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END