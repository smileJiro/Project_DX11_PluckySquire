#pragma once
#include "FloorWord.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

BEGIN(Client)
class CPrintFloorWord : public CFloorWord
{

protected:
	explicit CPrintFloorWord(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CPrintFloorWord(const CPrintFloorWord& _Prototype);
	virtual ~CPrintFloorWord() = default;

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
	static CPrintFloorWord*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() override;


private:
	void					Add_Amount(_float _fTimeDelta);

private:
	_float2					m_vRenderPos = { 0.f, 0.f };
	_tchar					m_tFloorWord[MAX_PATH] = {};
	_float					m_fAmount = { 0.f };
	_bool					m_isFadeIn = { false };
	_bool					m_isFadeInComplete = { false };
	wstring					m_strSFX = { L"" };
	wstring					m_strSection = { L"" };
};

END