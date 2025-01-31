#pragma once
#include "PartObject.h"

BEGIN(Engine)

/* 모든 파티클의 부모 클래스 */
/* 한 종류의 파티클만 나온다 */
/* Shader Component만 보유 */
/* 기본 함수들 보유 */

class ENGINE_DLL CEmitter : public CPartObject
{
public:
	enum EFFECT_TYPE { SPRITE, MESH, EFFECT, NONE };

public:
	typedef struct tagParticleEmitterDesc : public CPartObject::PARTOBJECT_DESC
	{
		_uint iProtoShaderLevel;
		const _tchar* szShaderTag = L"";
	} PARTICLE_EMITTER_DESC;

protected:
	CEmitter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CEmitter(const CEmitter& _Prototype);
	virtual ~CEmitter() = default;

public:
	static void SetID_3D(_int _iID) { s_iRG_3D = _iID; }
	static void SetID_2D(_int _iID) { s_iRG_2D = _iID; }
	static void SetID_Effect(_int _iID) { s_iRGP_EFFECT = _iID; }

public:
	virtual HRESULT				Initialize_Prototype(const json& _jsonInfo);
	virtual HRESULT				Initialize(void* _pArg) override;
	//virtual void				Priority_Update(_float _fTimeDelta) override;
	virtual void				Update(_float _fTimeDelta) override;
	virtual void				Late_Update(_float _fTimeDelta) override;

public:
	virtual	void				Reset(); 

public:
	const EFFECT_TYPE Get_Type() const { return m_eEffectType; }
	_uint				Get_EventID() const { return m_iEventID; }

protected:
	static _int			s_iRG_3D;
	static _int			s_iRG_2D;
	static _int			s_iRGP_EFFECT;

	_float4x4			m_IdentityMatrix = {};


protected:
	class CShader* m_pShaderCom = { nullptr };			

protected:
	EFFECT_TYPE	m_eEffectType = { NONE };			// Sprite or Mesh?
	_uint			m_iShaderPass = 0;					// Particle 설정 여부에 따라서 Pass도 바뀐다.
	_bool			m_isFollowParent = { true };		// System에 따라 위치가 결정? World 좌표 그대로로 설정? 여부
	/*_bool			m_isProgress = { true };*/			
	// 이벤트로 인해 Particle 진행여부는 Active로 판단.
	_uint			m_iEventID = { 0 };					// 파티클이 진행될 ID
	_float			m_fEventTime = { 0.f };				// 이벤트 총 진행시간
	_float			m_fAccTime = { 0.f };				// 진행시간
	_uint			m_iLoopTime = { 0 };				// 루프 횟수, 0 일경우 = 무한루프
	_uint			m_iAccLoop = { 0 };



protected:
	virtual void Active_OnEnable() override;
	virtual void Active_OnDisable() override;

protected:
	virtual HRESULT Ready_Components(const PARTICLE_EMITTER_DESC* _pDesc);



public:
	virtual CGameObject* Clone(void* _pArg) = 0;
	virtual void		 Free() override;
	virtual HRESULT		 Cleanup_DeadReferences() = 0;

#ifdef _DEBUG
public:
	virtual void				Tool_Update(_float fTimeDelta);
	virtual void				Tool_Setting();
	virtual HRESULT				Save(json& _jsonOut);

	_bool						Is_ToolChanged() { if (m_isToolChanged) { m_isToolChanged = false; return true; } else return false; }

protected:
	_bool						m_isToolProgress = { true };
	_bool						m_isToolChanged = { false };
#endif

};

END

BEGIN(Engine)

END