#pragma once
#include "MapObject.h"
#include "3DModel.h"


BEGIN(Client)

class C3DMapObject abstract : public CMapObject
{
public:
	typedef struct tag3DMapObjectDesc : public CMapObject::MAPOBJ_DESC
	{
		// 오버라이드 정보가 있는지?
		_bool isOverride = false;
		// 프러스텀 컬링할지 여부를 판단한다.
		_bool isCulling = true;
		_uint iSksp = 0;
		_string strSkspTag = "";
	}MAPOBJ_3D_DESC;

protected:
	C3DMapObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	C3DMapObject(const C3DMapObject& _Prototype);
	virtual ~C3DMapObject() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* _pArg) override;


	virtual void					After_Initialize() override;


	virtual void					Priority_Update(_float _fTimeDelta) override;
	virtual void					Update(_float _fTimeDelta) override;
	virtual void					Late_Update(_float _fTimeDelta) override;
	virtual HRESULT					Render() override;

	virtual void					Check_FrustumCulling() override;
																																									// 서브스레드를 쓰는 경우, Const Buffer를 Unmap하는 행위가 스레드 언세이프하므로, 추후에 해줘야함. 
	virtual void					Set_MaterialConstBuffer_Albedo(_uint _iMaterialIndex, Engine::C3DModel::COLOR_SHADER_MODE _eColorMode, _float4 _fAlbedoColor, _bool _isUpdate = true);
	virtual void					Update_ConstBuffer();

	_bool							Is_Sksp() { return m_isSksp; }
protected:
	_uint							m_iDiffuseIndex = 0;

	_bool							m_isCulling = true;
	_bool							m_isSksp = false;

public:
	virtual void			Free() override;
};
END
