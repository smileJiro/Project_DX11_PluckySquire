#pragma once
#include "Base.h"
#include "Model.h"
BEGIN(Engine)
class CBinaryFbx_Manager final : public CBase
{
private:
	CBinaryFbx_Manager();
	virtual ~CBinaryFbx_Manager() = default;
public:
	HRESULT ConvertToBinary(CModel::TYPE _eModelType, const _char* _szFbxPath, const _tchar* _szSaveBinaryPath); // fbx를 binary 파일로 저장.
	HRESULT LoadToBinary_NonAnim(const _tchar* _szBinaryPath, NONANIM_FBX* _pOut); // 참조로 반환, 받는쪽에선 복사해서받아.
	HRESULT LoadToBinary_Anim(const _tchar* _szBinaryPath, ANIM_FBX* _pOut); // 참조로 반환, 받는쪽에선 복사해서받아.

private:
	HRESULT Save_NonAnimBinary(const _tchar* _szSaveBinaryPath);


private:
	Assimp::Importer m_Importer;
	const aiScene* m_pAIScene;

public:
	static CBinaryFbx_Manager* Create();
	virtual void Free() override;
};

END
