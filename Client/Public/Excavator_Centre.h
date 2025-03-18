#pragma once
#include "Character.h"
BEGIN(Client)
class CExcavator_Centre final : public CCharacter
{
public:
	enum CENTRE_PART { CENTRE_BG, CENTRE_TROOPER_L, CENTRE_TROOPER_R, CENTRE_CENTRE, CENTRE_REGULATOR, CENTRE_SUTTER, CENTRE_SWITCH_0, CENTRE_SWITCH_1, CENTRE_SWITCH_2, CENTRE_SAW, CENTRE_LAST }; // PART인덱스이자 애니메이션 인덱스임
	typedef struct tagCentreDesc : public CCharacter::CHARACTER_DESC
	{

	}CENTRE_DESC;
private:
	CExcavator_Centre(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CExcavator_Centre(const CExcavator_Centre& _Prototype);
	virtual ~CExcavator_Centre() = default;

public:
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta)override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;

private:
	HRESULT					Ready_PartObjects(CENTRE_DESC* _pDesc);

private: /* Centre Switch Count */
	void Check_SwitchCount();
	_bool m_isFirst = false;
	_int m_iSwitchCount = 0;


public:
	static CExcavator_Centre* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg) override;
	void Free() override;
};

END