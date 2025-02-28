#pragma once
#include "ModelObject.h"
BEGIN(Map_Tool)

class CSample_Skechspace final : public CModelObject
{
public :
	typedef struct tagSampleSkechspace : CModelObject::MODELOBJECT_DESC
	{
		_bool isPreview = false;
	}SAMPLE_SKSP_DESC;
private:
	CSample_Skechspace(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSample_Skechspace(const CSample_Skechspace& _Prototype);
	virtual ~CSample_Skechspace() = default;
public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;
	HRESULT					Render_Plag();
	HRESULT					Render_Cup();
public :
	_bool					m_isPreView = false;
	SKSP_TYPE				m_eSkspType = SKSP_DEFAULT;

public:
	static CSample_Skechspace* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;

};

END