#pragma once
#include "Character.h"
/* 파트 오브젝트로 Threads를 소유하고 렌더시키는 클래스 */
BEGIN(Client)
class CExcavator_Tread final : public CCharacter
{
public:
	enum TREAD_PART { TREAD_INNER, TREAD_OUTER, TREAD_LINES, TREAD_WHEEL_L, TREAD_WHEEL_M, TREAD_WHEEL_R,  TREAD_LUGNUTS_L, TREAD_LUGNUTS_M, TREAD_LUGNUTS_R, TREAD_LAST }; // PART인덱스이자 애니메이션 인덱스임
	typedef struct tagTreadDesc : public CCharacter::CHARACTER_DESC
	{

	}TREAD_DESC;
private:
	CExcavator_Tread(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CExcavator_Tread(const CExcavator_Tread& _Prototype);
	virtual ~CExcavator_Tread() = default;

public:
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta)override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;

private:
	HRESULT					Ready_PartObjects(TREAD_DESC* _pDesc);

public:
	static CExcavator_Tread* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg) override;
	void Free() override;

};

END