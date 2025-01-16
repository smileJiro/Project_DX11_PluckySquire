#pragma once
#include "GameObject.h"

BEGIN(Engine)
class CPartObject;
class CComponent;
class ENGINE_DLL CContainerObject abstract : public CGameObject
{
public:
	typedef struct tagContainerObjectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		// 파트 오브젝트의 개수
		_uint			iNumPartObjects; 
	}CONTAINEROBJ_DESC;

protected:
	CContainerObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CContainerObject(const CContainerObject& _Prototype);
	virtual ~CContainerObject() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* _pArg) override;
	virtual void		Priority_Update(_float _fTimeDelta) override;
	virtual void		Update(_float _fTimeDelta) override;
	virtual void		Late_Update(_float _fTimeDelta) override;
	virtual HRESULT		Render() override;

public:
	CComponent*			Find_Part_Component(_uint _iPartObjectIndex, const _wstring& _strPartComponentTag);
	HRESULT				Add_PartObject(CGameObject* _pPartObject);

public:
	// Get
	CGameObject*		Get_PartObject(_int _iPartObjectIndex);
	_int				Get_NumPartObjects() {
		if (true == m_PartObjects.empty())
			return 0;
		return (_int)m_PartObjects.size();
	}

protected:
	vector<CGameObject*> m_PartObjects;

public:
	virtual void Free() override;
	HRESULT Cleanup_DeadReferences() override;

#ifdef _DEBUG
public:
	HRESULT Imgui_Render_ObjectInfos() override;
#endif // _DEBUG
};

END