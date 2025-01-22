#include "stdafx.h"
#include "Test2DModel.h"

#include "ToolAnimation2D.h"

CTest2DModel::CTest2DModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:C2DModel(_pDevice, _pContext)
{
}

CTest2DModel::CTest2DModel(const CTest2DModel& _Prototype)
	:C2DModel(_Prototype)
{
}



HRESULT CTest2DModel::Initialize_Prototype_FromJsonFile(const _char* _szRawDataDirPath)
{

	//모든 json파일 순회하면서 읽음. 
	//Type이 PaperSprite인 경우와 PaperFlipBook인 경우로 컨테이너를 나눠서 저장.
	//	PaperFlipBook 컨테이너를 순회하면서	Animation2D를 생성.
	//    Animation2D를 생성할 때 PaperSprite 컨테이너에서 LookUp해서 CSpriteFrame생성.

	map<string, json> jPaperFlipBooks;
	map<string, json> jPaperSprites;


	std::filesystem::path path;
	path = _szRawDataDirPath;
	json jFile;
	for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
		if (entry.path().extension() == ".json")
		{
			//cout << entry.path().string() << endl;
			std::ifstream input_file(entry.path());
			if (!input_file.is_open())
				return E_FAIL;
			input_file >> jFile;
			input_file.close();

			for (auto& jObj : jFile)
			{
				string strType = jObj["Type"];
				if (strType._Equal("PaperSprite"))
				{
					string strName = jObj["Name"];

					jPaperSprites.insert({ strName, jObj });
				}
				else if (strType._Equal("PaperFlipbook"))
				{
					string strName = jObj["Name"];

					jPaperFlipBooks.insert({ strName,jObj });
				}
				else
				{
					continue;
				}
			}

		}
		else if (entry.path().extension() == ".png")
		{
			if (m_Textures.find(entry.path().filename().replace_extension().string()) != m_Textures.end())
				continue;
			ID3D11ShaderResourceView* pSRV = { nullptr };
			HRESULT hr = DirectX::CreateWICTextureFromFile(m_pDevice, entry.path().c_str(), nullptr, &pSRV);
			if (FAILED(hr))
				return E_FAIL;
			CTexture* pTexture = CTexture::Create(m_pDevice, m_pContext );
			if (nullptr == pTexture)
				return E_FAIL;
			string strTextureName = entry.path().filename().replace_extension().string();
			wstring wstrTextureName = StringToWstring(strTextureName);
			pTexture->Add_Texture(pSRV, wstrTextureName.c_str());
			auto result = m_Textures.insert({ strTextureName, pTexture });
			if (result.second == false)
				Safe_Release(pTexture);
		}
	}


	m_eAnimType = jPaperFlipBooks.size() > 0 ? ANIM : NONANIM;

	if (ANIM == m_eAnimType)
	{
		for (auto& j : jPaperFlipBooks)
		{
			string strName = j.second["Name"];

			m_Animation2Ds.push_back(CToolAnimation2D::Create(m_pDevice, m_pContext, j.second, jPaperSprites, m_Textures));
		}
	}
	else
	{
		for (auto& pTex : m_Textures)
		{
			m_NonAnimTextures.push_back(pTex.second);
		}
	}
	return S_OK;
}

HRESULT CTest2DModel::Initialize_Prototype(const _char* _szModel2DFilePath)
{
	std::ifstream inFile(_szModel2DFilePath, std::ios::binary);
	if (!inFile) {
		string str = "파일을 열 수 없습니다.";
		str += _szModel2DFilePath;
		MessageBoxA(NULL, str.c_str(), "에러", MB_OK);
		return E_FAIL;
	}
	_char		szDrive[MAX_PATH] = "";
	_char		szDirectory[MAX_PATH] = "";
	_splitpath_s(_szModel2DFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);
	strcat_s(szDrive, szDirectory);

	//AllTextures
	_uint iCount = 0;
	inFile.read(reinterpret_cast<char*>(&iCount), sizeof(_uint));
	for (_uint i = 0; i < iCount; i++)
	{
		//TextureName Length
		_uint iLength = 0;
		inFile.read(reinterpret_cast<char*>(&iLength), sizeof(_uint));
		_char szTextureName[MAX_PATH] = "";
		inFile.read(szTextureName, iLength);
		szTextureName[iLength] = '\0';
		if (m_Textures.find(szTextureName) != m_Textures.end())
			continue;
		ID3D11ShaderResourceView* pSRV = { nullptr };
		std::filesystem::path path = szDrive;
		path += szTextureName;
		path += ".png";
		if (FAILED(DirectX::CreateWICTextureFromFile(m_pDevice, path.c_str(), nullptr, &pSRV)))
			return E_FAIL;
		CTexture* pTexture = CTexture::Create(m_pDevice, m_pContext);
		if (nullptr == pTexture)
			return E_FAIL;
		pTexture->Add_Texture(pSRV, path.filename().replace_extension().wstring());
		m_Textures.insert({ szTextureName,pTexture });
	}

	

	//Animation2Ds
	inFile.read(reinterpret_cast<char*>(&iCount), sizeof(_uint));
	m_Animation2Ds.reserve(iCount);
	for (_uint i = 0; i < iCount; i++)
	{
		CToolAnimation2D* pAnimation = CToolAnimation2D::Create(m_pDevice, m_pContext, szDrive, inFile, m_Textures);
		if (nullptr == pAnimation)
			return E_FAIL;
		m_Animation2Ds.push_back(pAnimation);
	}
	m_eAnimType = m_Animation2Ds.size() > 0 ? ANIM : NONANIM;
	//NonANimTextures
	inFile.read(reinterpret_cast<char*>(&iCount), sizeof(_uint));
	m_NonAnimTextures.reserve(iCount);
	for (_uint i = 0; i < iCount; i++)
	{
		_uint iLength = 0;
		inFile.read(reinterpret_cast<char*>(&iLength), sizeof(_uint));
		_char* pTextureName = new char[iLength + 1];
		inFile.read(pTextureName, iLength);
		pTextureName[iLength] = '\0';
		const auto& pairTexture =m_Textures.find(pTextureName);
		if (pairTexture != m_Textures.end())
		{
			m_NonAnimTextures.push_back(pairTexture->second);
		}

	}
	//NonAnimSpriteStartUV
	inFile.read(reinterpret_cast<char*>(&m_vNonAnimSpriteStartUV), sizeof(_float2));
	//NonAnimSpriteEndUV
	inFile.read(reinterpret_cast<char*>(&m_vNonAnimSpriteEndUV), sizeof(_float2));
	return S_OK;
}

HRESULT CTest2DModel::Export_Model(ofstream& _outfile)
{
	_uint iCount = 0;
	//AllTextures
	iCount = m_Textures.size();
	_outfile.write(reinterpret_cast<const char*>(&iCount), sizeof(_uint));
	for (auto& pTexture : m_Textures)
	{
		string strTextureName = pTexture.first;
		iCount = strTextureName.length();
		_outfile.write(reinterpret_cast<const char*>(&iCount), sizeof(_uint));
		_outfile.write(strTextureName.c_str(), iCount);
	}

	//Animations
	iCount = m_Animation2Ds.size();
	_outfile.write(reinterpret_cast<const char*>(&iCount), sizeof(_uint));
	for (auto& pAnimation : m_Animation2Ds)
	{
		static_cast<CToolAnimation2D*>(pAnimation)->Export(_outfile);
	}

	//NonAnimTextures
	iCount = m_NonAnimTextures.size();
	_outfile.write(reinterpret_cast<const char*>(&iCount), sizeof(_uint));
	for (auto& pTexture : m_NonAnimTextures)
	{
		const wstring* pTextureName = pTexture->Get_SRVName(0);
		string strTextureName = WstringToString(*pTextureName);
		iCount = strTextureName.length();
		_outfile.write(reinterpret_cast<const char*>(&iCount), sizeof(_uint));
		_outfile.write(strTextureName.c_str(), iCount);
	}

	//NonAnimSpriteStartUV
	_outfile.write(reinterpret_cast<const char*>(&m_vNonAnimSpriteStartUV), sizeof(_float2));
	//NonAnimSpriteEndUV
	_outfile.write(reinterpret_cast<const char*>(&m_vNonAnimSpriteEndUV), sizeof(_float2));

	return S_OK;
}

void CTest2DModel::Get_TextureNames(set<wstring>& _outTextureNames)
{
	for (auto& pTexture : m_Textures)
	{
		_outTextureNames.insert(*pTexture.second->Get_SRVName(0));
	}
}

CTest2DModel* CTest2DModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _bool _bRawData, const _char* _pPath)
{
	CTest2DModel* pInstance = new CTest2DModel(pDevice, pContext);

	if (_bRawData)
	{
		if (FAILED(pInstance->Initialize_Prototype_FromJsonFile(_pPath)))
		{
			MSG_BOX("Failed to Created : Test2DModel");
			Safe_Release(pInstance);
		}
	}
	else
	{
		if (FAILED(pInstance->Initialize_Prototype(_pPath)))
		{
			MSG_BOX("Failed to Created : Test2DModel");
			Safe_Release(pInstance);
		}
	}

	return pInstance;
}

CComponent* CTest2DModel::Clone(void* _pArg)
{
	CTest2DModel* pInstance = new CTest2DModel(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : Test2DModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTest2DModel::Free()
{
	__super::Free();
}
