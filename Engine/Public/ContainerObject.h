#pragma once
#include "ActorObject.h"

BEGIN(Engine)
class CPartObject;
class CComponent;
class ENGINE_DLL CContainerObject abstract : public CActorObject
{
public:
	enum PART { PART_BODY, PART_LAST };
public:
	typedef struct tagContainerObjectDesc : public CActorObject::ACTOROBJECT_DESC
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
	virtual HRESULT		Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition = nullptr) override;
	HRESULT				Register_RenderGroup(_uint _iGroupId, _uint _iPriorityID) override;

public:
	CComponent*			Find_Part_Component(_uint _iPartObjectIndex, const _wstring& _strPartComponentTag);
	HRESULT				Add_PartObject(CGameObject* _pPartObject);
	HRESULT				Attach_PartObject(CPartObject* _pPartObject, _uint _iPartID);
	HRESULT				Detach_PartObject(_uint _iPartID);
public:
	// Get
	CGameObject*		Get_PartObject(_int _iPartObjectIndex);
	_int				Get_NumPartObjects() {
		if (true == m_PartObjects.empty())
			return 0;
		return (_int)m_PartObjects.size();
	}
	_bool Is_PartActive(_uint _iPartID) {
		if (m_PartObjects[_iPartID] == nullptr)
			return false;
		return m_PartObjects[_iPartID]->Is_Active();
	}
	//Set
	void				Set_PartActive(_uint _iPartID, _bool _bValue);
protected:
	vector<CGameObject*> m_PartObjects;

public:
	void Free() override;
	HRESULT Cleanup_DeadReferences() override;

#ifdef _DEBUG
public:
	HRESULT Imgui_Render_ObjectInfos() override;
#endif // _DEBUG
};

END