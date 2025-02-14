#pragma once
#include "ModelObject.h"

BEGIN(Client)
class CFallingRock final : public CModelObject
{
public:
	typedef struct tagFallingRockDesc : CModelObject::MODELOBJECT_DESC
	{
		_float fFallDownEndY = {};
	}FALLINGROCK_DESC;
public:
	enum STATE { STATE_FALLDOWN, STATE_BOUND, STATE_LAST };
private:
	CFallingRock(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CFallingRock(const CFallingRock& _Prototype);
	virtual ~CFallingRock() = default;

public:
	virtual HRESULT			Initialize_Prototype(); // 프로토 타입 전용 Initialize
	virtual HRESULT			Initialize(void* _pArg); // 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	STATE					m_ePreState = STATE::STATE_LAST;
	STATE					m_eCurState = STATE::STATE_FALLDOWN;

private: /* FallDown */
	_float					m_fFallDownEndY;

private: /* Bound */
	_float2					m_vBoundCycle;

	
public:
	static CFallingRock*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject*			Clone(void* _pArg) override;
	void					Free() override;
};

END