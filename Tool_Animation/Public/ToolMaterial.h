#pragma once
#include "Material.h"

BEGIN(AnimTool)
class CToolMaterial :
    public CMaterial
{
private:
	CToolMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CToolMaterial() = default;

public:
	void						Export(ofstream& _outfile);

	void						Get_TextureNames(set<wstring>& _outTextureNames);
	_uint						Get_TextureCount(_uint iTexType);
public:
	static CToolMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* szDirPath, ifstream& inFile);
	CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END