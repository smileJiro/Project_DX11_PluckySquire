#pragma once
#include "State_Sneak.h"

BEGIN(Client)


//소리를 듣고 경계하는 상태
class CSneak_AwareState final : public CState_Sneak
{
private:
	CSneak_AwareState();
	virtual ~CSneak_AwareState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

public:
	void	Set_Sneak_AwarePos(_fvector _vPosition);

private:
	_float m_fAccTime = {};
	_float m_fRecognizeTime = {};
	_bool m_isRecognize = {};
	_bool m_isConvert = {};
	_bool m_isRenew = {};
	_bool m_isTurn = {};

public:
	static CSneak_AwareState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END