#pragma once

#include "Map_Tool_Defines.h"
#include "ModelObject.h"
#include "2DMapObjectInfo.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
class CCollider;
END


BEGIN(Map_Tool)

class C2DMapObject final : public CModelObject
{
public :

public:
	typedef struct tag2DMapObjectDesc : CModelObject::MODELOBJECT_DESC
	{
		_bool		isLoad = true;
		_wstring	strProtoTag;
		_float2		fDefaultPosition = {0.f, 0.f};
		_float2		fRenderTargetSize;
		_int		iRenderGroupID_2D;
		_int		iPriorityID_2D;

		C2DMapObjectInfo* pInfo = nullptr;
	}MAPOBJ_2D_DESC;
private:
	C2DMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	C2DMapObject(const C2DMapObject& Prototype);
	virtual ~C2DMapObject() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

	void						Set_OffsetPos(_float2 _fPos);
	_bool						IsCursor_In(_float2 _fCursorPos);
	const _wstring&				Get_Key() { return m_strKey; }
	const _wstring&				Get_ModelName() { return m_strModelName; }
	_bool						Is_ModelLoad() { return m_isModelLoad; }

	HRESULT						Export(HANDLE hFile);
	HRESULT						Import(HANDLE hFile,vector<C2DMapObjectInfo*>& _ModelInfos);

	virtual _vector				Get_FinalPosition() const override; 
	
	HRESULT						Update_Model_Index();
	
	void						Set_DefaultPosition(_float2 _fPosition) { m_fDefaultPosition = _fPosition; }
	_float2						Get_DefaultPosition() { return m_fDefaultPosition; }

private:
	_float4x4 m_ProjMatrix, m_ViewMatrix;
	C2DMapObjectInfo* m_pModelInfo = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

	_wstring		m_strKey = {};
	_wstring		m_strModelName = {};
	_float2			m_fTextureOffsetSize;
	_float2			m_fRenderTargetSize; 
	_bool			m_isModelLoad = false;
	
	_float2			m_fDefaultPosition = {};

	_int			m_iRenderGroupID;
	_int			m_iPriorityID;

public:
	static C2DMapObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static C2DMapObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HANDLE _hFile, vector<C2DMapObjectInfo*>& _ModelInfos);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;



	// CUIObject을(를) 통해 상속됨
	virtual HRESULT Cleanup_DeadReferences() override;

};

END