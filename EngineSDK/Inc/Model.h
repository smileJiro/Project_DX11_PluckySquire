#pragma once
#include "Component.h"
BEGIN(Engine)
class CShader;
class ENGINE_DLL CModel :
    public CComponent
{
public:
	enum TYPE { NONANIM, ANIM, LAST, };
protected:
	CModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CModel(const CModel& _Prototype);
	virtual ~CModel() = default;
public:
	virtual HRESULT			Render(CShader* _Shader, _uint _iShaderPass)abstract;
	virtual _bool Play_Animation(_float fTimeDelta) {return false;}

	virtual void Set_AnimationLoop(_uint iIdx, _bool bIsLoop) abstract;
	virtual void Set_Animation(_uint iIdx) abstract;
	virtual void Switch_Animation(_uint iIdx) abstract;

	TYPE Get_AnimType() { return m_eModelType; }
	_bool Is_AnimModel() { return m_eModelType == TYPE::ANIM; };

protected:
	TYPE				m_eModelType = TYPE::LAST;

public:
	virtual void Free() override;
};

END