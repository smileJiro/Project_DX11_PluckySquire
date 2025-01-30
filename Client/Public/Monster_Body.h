#pragma once
#include "ModelObject.h"

BEGIN(Engine)
class CDebugDraw_For_Client;
END

BEGIN(Client)
class CFSM;
class CDetectionField;
class CMonster_Body : public CModelObject
{
private:
	CMonster_Body(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMonster_Body(const CMonster_Body& _Prototype);
	virtual ~CMonster_Body() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Cleanup_DeadReferences() override;
	virtual void Active_OnEnable() override;
	virtual void Active_OnDisable() override;

protected:
	HRESULT Ready_Desc(void** _pArg);
	//virtual HRESULT Ready_Components() = 0;
	//virtual HRESULT Ready_PartObjects() = 0;


public:
	static CMonster_Body* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg);
	virtual void Free() override;
};

END