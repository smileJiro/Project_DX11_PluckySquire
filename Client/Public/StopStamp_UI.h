#pragma once
#include "UI.h"
#include "Client_Defines.h"
#include "UI_Manager.h"


BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

BEGIN(Client)
class CStopStamp_UI final : public CUI
{

protected:
	explicit CStopStamp_UI(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CStopStamp_UI(const CStopStamp_UI& _Prototype);
	virtual ~CStopStamp_UI() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	CPlayer::PLAYER_PART	m_ePreStamp = { CPlayer::PLAYER_PART::PLAYER_PART_LAST };
	_bool					m_isSmall = { false };
	_bool					m_isBig = { false };
	_bool					m_isScaling = { false };

	_bool					m_isFirstPositionAdjusted = { false };
	_bool					m_isRenderStamp = { false };

private:
	void					ChangeStamp(_float _fTimeDelta);


protected:
	virtual HRESULT			Ready_Components() override;

public:
	static CStopStamp_UI*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() override;

};
END

