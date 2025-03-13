#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

class CStampKey_1 final : public CUI
{

protected:
	explicit CStampKey_1(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CStampKey_1(const CStampKey_1& _Prototype);
	virtual ~CStampKey_1() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;


protected:
	virtual HRESULT			Ready_Components() override;

private:
	void					Change_StampKeyWord();

public:
	static CStampKey_1*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() override;


private:
	wstring					m_strStampInfo = { TEXT("¹Ù²Ù±â") };
	CPlayer::PLAYER_PART	m_ePreStampChoose = { CPlayer::PLAYER_PART::PLAYER_PART_LAST };
	_uint					m_iTextureKind = { 0 };
};

