#pragma once
#include "GameObject.h"

BEGIN(Engine)

/* 모든 파티클의 부모 클래스 */
/* 한 종류의 파티클만 나온다 */
/* Shader Component만 보유 */
/* 기본 함수들 보유 */

class ENGINE_DLL CParticle_Emitter : public CGameObject
{
public:
	enum PARTICLE_TYPE { SPRITE, MESH, NONE };

public:
	typedef struct tagParticleEmitterDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_uint iProtoShaderLevel;
		const _tchar* szShaderTag = L"";
	} PARTICLE_EMITTER_DESC;

protected:
	CParticle_Emitter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CParticle_Emitter(const CParticle_Emitter& _Prototype);
	virtual ~CParticle_Emitter() = default;

public:
	const PARTICLE_TYPE Get_Type() const { return m_eType; }

protected:
	class CShader* m_pShaderCom = { nullptr };

protected:
	_uint			m_iShaderPass = 0;
	PARTICLE_TYPE	m_eType = { NONE };

protected:
	virtual HRESULT Ready_Components(const PARTICLE_EMITTER_DESC* _pDesc);


public:
	virtual CGameObject* Clone(void* _pArg) = 0;
	virtual void		 Free() override;
	virtual HRESULT		 Cleanup_DeadReferences() = 0;

#ifdef _DEBUG
	virtual void				Tool_Update(_float fTimeDelta);
	virtual void				Tool_Setting();
	virtual HRESULT				Save(json& _jsonOut) { return S_OK; }
#endif

};

END