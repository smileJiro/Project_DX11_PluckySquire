#pragma once
#include "UI.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

class CStampKey_Q final : public CUI
{
protected:
	explicit CStampKey_Q(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CStampKey_Q(const CStampKey_Q& _Prototype);
	virtual ~CStampKey_Q() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;


protected:
	virtual HRESULT			Ready_Components() override;

public:
	static CStampKey_Q*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() override;

private:
	void					ChangeStampWord();

private:
	wstring					m_strStampKeyInfomation = { L"" };
	_uint					m_iPreStamp = { 2 };


};

