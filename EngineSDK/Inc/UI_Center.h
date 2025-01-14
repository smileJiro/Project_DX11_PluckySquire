#pragma once
#include "UI.h"

/* 1. 유아이의 포지션 중점을 정 중앙 기준으로 설정하는 유형의 유아이 */

BEGIN(Engine)
class ENGINE_DLL CUI_Center abstract : public CUI
{
protected:
	CUI_Center(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CUI_Center(const CUI_Center& _Prototype);
	virtual ~CUI_Center() = default;

public:
	virtual void Apply_UIPosition() override;// 기본 좌상단 기준으로 정렬하지만, 특정 ui는 포지션을 중점 기준으로 정렬하게 세팅하기 위해 virtual
	
protected: /* Interaction */
	virtual void Check_MouseOn() override;			// 현재 자기 자신 위에 마우스가 올라와있는지 체크,
	virtual void Check_MouseOn_Circle() override;			// 현재 자기 자신 위에 마우스가 올라와있는지 체크,

public:
	virtual void Free() override;
	virtual HRESULT Safe_Release_DeadObjects() override;
};

END