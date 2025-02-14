#pragma once
#include "UI.h"

BEGIN(Client)
class CNarration_Anim final: public CUI
{
public:


private:
	CNarration_Anim(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CNarration_Anim(const CNarration_Anim& _Prototype);
	virtual ~CNarration_Anim() = default;



public:
	virtual HRESULT				Initialize_Prototype();								// 프로토 타입 전용 Initialize
	virtual HRESULT				Initialize(void* _pArg);							// 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void				Priority_Update(_float _fTimeDelta);				// 특정개체에 대한 참조가 빈번한 객체들이나, 등등의 우선 업데이트 되어야하는 녀석들.
	virtual void				Update(_float _fTimeDelta);
	virtual void				Late_Update(_float _fTimeDelta);
	virtual HRESULT				Render();




protected:
	virtual HRESULT Ready_Components() override;


public:
	static CNarration_Anim*			Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*		Clone(void* _pArg) override; // Clone() 프로토 타입이나 객체의 복사시 사용된다.
	virtual void				Free() override;
	virtual HRESULT				Cleanup_DeadReferences() override; // 참조 중인 게임오브젝트들 중 죽은 Dead상태인 오브젝트를 체크해서 참조해제.(액티브 false인 애들때매 만듬)
	void						StartAnimation();
	_bool						Get_PlayAnimation() { return m_isPlayAnimation; }

private:
	void						On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx);



private:
	_wstring					m_strSectionId = {};
	_wstring					m_strAnimationId = {};
	_float2						m_vPos = { 0.f, 0.f };
	_float						m_fWaitingTime = { 0.f };
	_float2						m_fAnimationScale = { 0.f, 0.f };
	_bool						m_isPlayAnimation = { false };
};

END
