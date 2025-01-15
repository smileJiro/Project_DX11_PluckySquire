#pragma once
#include "Model.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
class C2DModel :
    public CModel
{
public:
    typedef struct tag2DModelDesc
    {
        string strTextureTag;
        json j2DModelData;
    }TWODMODEL_DESC;
private:
    C2DModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    C2DModel(const C2DModel& _Prototype);
    virtual ~C2DModel() = default;
public:
    HRESULT			Initialize(void* _pDesc);

    virtual HRESULT			Render(CShader* _Shader, _uint _iShaderPass)override;
    virtual _bool			Play_Animation(_float fTimeDelta)override;

    virtual void Set_AnimationLoop(_uint iIdx, _bool bIsLoop)override;
    virtual void Set_Animation(_uint iIdx)override;
    virtual void Switch_Animation(_uint iIdx)override;


private:
    CVIBuffer_Rect* m_pVIBufferCom = nullptr;
    _uint					m_iSRVIndex = 0;
	vector<CTexture*> m_TextureComs ;

public:
    static C2DModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent* Clone(void* _pArg) override;
    virtual void Free() override;
};

END