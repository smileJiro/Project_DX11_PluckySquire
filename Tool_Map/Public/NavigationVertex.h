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
    CNavigationVertex(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CNavigationVertex(const CNavigationVertex& _Prototype);
    virtual ~CNavigationVertex() = default;

public :
    HRESULT             Initialize(const _float3& _vVerTexPos);
    virtual void	    Priority_Update(_float _fTimeDelta) override;
    virtual void	    Update(_float _fTimeDelta) override;
    virtual void	    Late_Update(_float _fTimeDelta) override;
    virtual HRESULT     Render() override;
    void                Add_Cell(CEditableCell* _pCell) { m_vecIncludeCell.push_back(_pCell); }
    _float3&            Get_Pos() { return m_vPos; }
    virtual bool	    Check_Picking(_fvector _vRayPos, _fvector _vRayDir, _float* _fNewDist);

    void                Set_Mode(MODE _eMode) { m_eMode = _eMode; }
    void                Update_Pos();
    void                Delete_Vertex();

    _uint               Get_IncludeCellCount() { return (_uint)m_vecIncludeCell.size(); }

    void                Set_Index(_uint _iIdx) { m_iIdx = _iIdx; }
    _uint               Get_Index() { return m_iIdx; }

private :
    CCollider*                          m_pCollider;
    vector<CEditableCell*>              m_vecIncludeCell;
    _uint                               m_iIdx;
    _float4                             m_vColorArr[MODE::MODE_END];
    _float3                             m_vPos;
    _float4x4		                    m_matGizmoWorld;
    MODE                                m_eMode = NORMAL;

public :
    virtual void                        Free();
    virtual CGameObject*                Clone(void* _pArg);
    static CNavigationVertex*           Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _float3& _vVerTexPos);
    virtual                             HRESULT Cleanup_DeadReferences() override;



};


END