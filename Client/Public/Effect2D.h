#pragma once
#include "ModelObject.h"


BEGIN(Client)
class CEffect2D final : public CModelObject
{
public:
	typedef struct tagEffect2D : CModelObject::MODELOBJECT_DESC
	{
		
	}EFFECT2D_DESC;

private:
	CEffect2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CEffect2D(const CEffect2D& _Prototype);
	virtual ~CEffect2D() = default;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* _pArg);
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public: /* Effect 재생 */
	HRESULT					Play_Effect(const _wstring& _strSectionKey, _fmatrix _2DWorldMatrix, _float _fDelayTime = 0.0f, _uint _iAnimIndex = 0, _bool _isLoop = false, _float _fLifeTime = 0.0f);
	//HRESULT					Play_Effect(const _wstring& _strSectionKey, _fvector _vPos, _uint _EffectAnimIndex = 0, _float _fDelayTime = 0.0f, _bool _isLoop = false,  // 구현은 했으나 테스트는 못했음.
	//									 _float2 _vScale = { 1.0f, 1.0f }, _float _fRadianZ = 0.0f);																	   // 구현은 했으나 테스트는 못했음.
private:
	void					On_AnimEnd(COORDINATE _eCoord, _uint _iAnimIdx);

public:
	// Get 
	const _wstring&			Get_EffectModelPrototypeTag() const { return m_strEffectModelPrototypeTag; }
	LEVEL_ID				Get_EffectModelPrototypeLevelID() const { return m_eEffectModelPrototypeLevelID; }
	// Set 
private: 
	_wstring				m_strEffectModelPrototypeTag = {};
	LEVEL_ID				m_eEffectModelPrototypeLevelID = LEVEL_ID::LEVEL_END;
private: 
	_float2					m_vDelayTime = {};
	_bool					m_isLoop = false;
	_bool					m_isPlay = false;
	_uint					m_iCurAnimIndex = 0;
	_float2					m_vLifeTime = {}; /* 일반적으로 Effect는 자기 자신의 Anim 재생 후 소멸한다. Loop인 경우 vLifeTime을 사용한다.*/
	
private:
	HRESULT					Ready_Components(EFFECT2D_DESC* _pDesc);
private:
	/* Active 변경시 호출되는 함수 추가. */
	virtual void Active_OnEnable();
	virtual void Active_OnDisable();
public:
	static CEffect2D*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;

};
END
