#pragma once
#include "UI.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

class CInteraction_Key final : public CUI
{
protected:
	explicit CInteraction_Key(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CInteraction_Key(const CInteraction_Key& _Prototype);
	virtual ~CInteraction_Key() = default;

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
	static CInteraction_Key*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() override;

private:
	void					ChangeStampWord();

private:
	wstring					m_strStampKeyInfomation = { L"" };
	_uint					m_iPreStamp = { 2 };


};

