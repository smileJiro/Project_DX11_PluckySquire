#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CPartObject abstract : public CGameObject
{
public:
	typedef struct tagPartObjectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		const _float4x4* pParentMatrices[COORDINATE_LAST] = {};
	}PARTOBJECT_DESC;
protected:
	CPartObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPartObject(const CPartObject& _Prototype);
	virtual ~CPartObject() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	const _float4x4*		Get_FinalWorldMatrix_Ptr(COORDINATE _eCoord) { return &m_WorldMatrices[_eCoord]; }

protected:
	const _float4x4*		m_pParentMatrices[COORDINATE_LAST] = {}; // 부모의 월드 행렬의 주소
	_float4x4				m_WorldMatrices[COORDINATE_LAST] = {}; // 자기자신의 최종 행렬 

public:
	virtual void			Free() override;
	HRESULT					Safe_Release_DeadObjects() override; 
};

END