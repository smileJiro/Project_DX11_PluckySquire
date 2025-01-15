#pragma once

#include "GameObject.h"
#include "Map_Tool_Defines.h"

BEGIN(Engine)

class CCollider;

END

BEGIN(Map_Tool)

class CEditableCell;

class CNavigationVertex final :
    public CGameObject
{
public :
    enum MODE
    {
        NORMAL,
        SET,
        PICKING_VERTEX,
        PICKING_FIRST,
        PICKING_SECOND,
        PICKING_THIRD,
        MODE_END,
    };
private :
    CNavigationVertex(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CNavigationVertex(const CNavigationVertex& Prototype);
    virtual ~CNavigationVertex() = default;

public :
    HRESULT             Initialize(const _float3& vVerTexPos);
    virtual void	    Priority_Update(_float fTimeDelta) override;
    virtual void	    Update(_float fTimeDelta) override;
    virtual void	    Late_Update(_float fTimeDelta) override;
    virtual HRESULT     Render() override;
    void                Add_Cell(CEditableCell* pCell) { m_vecIncludeCell.push_back(pCell); }
    _float3&            Get_Pos() { return m_vPos; }
    virtual bool	    Check_Picking(_fvector vRayPos, _fvector vRayDir, _float* fNewDist);

    void                Set_Mode(MODE eMode) { m_eMode = eMode; }
    void                Update_Pos();
    void                Delete_Vertex();

    _uint               Get_IncludeCellCount() { return (_uint)m_vecIncludeCell.size(); }

    void                Set_Index(_uint iIdx) { m_iIdx = iIdx; }
    _uint               Get_Index() { return m_iIdx; }

private :
    _uint                               m_iIdx;
    CCollider*                          m_pCollider;
    vector<CEditableCell*>              m_vecIncludeCell;
    _float4                             m_vColorArr[MODE::MODE_END];
    _float3                             m_vPos;
    _float4x4		                    m_matGizmoWorld;
    MODE                                m_eMode = NORMAL;

public :
    virtual void                        Free();
    virtual CGameObject*                Clone(void* pArg);
    static CNavigationVertex*           Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3& vVerTexPos);

};


END