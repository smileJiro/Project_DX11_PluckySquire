#include "Engine_Function.h"
#include "Engine_Defines.h"

XMVECTOR Engine::ExtractQuaternionFromMatrix(const XMMATRIX& _Matrix)
{
	// 상위 3x3 부분을 회전 행렬로 사용
	XMMATRIX rotationMatrix = XMMatrixRotationMatrix(M);

	// 3x3 회전 행렬에서 쿼터니언을 추출
	XMVECTOR quaternion = XMQuaternionRotationMatrix(rotationMatrix);

	return quaternion;
}
string Engine::MatrixToString(_float4x4 vMatrix)
{
	string strMatrix;
	strMatrix += std::to_string(vMatrix._11) + " " + std::to_string(vMatrix._12) + " " + std::to_string(vMatrix._13) + " " + std::to_string(vMatrix._14) + "\n";
	strMatrix += std::to_string(vMatrix._21) + " " + std::to_string(vMatrix._22) + " " + std::to_string(vMatrix._23) + " " + std::to_string(vMatrix._24) + "\n";
	strMatrix += std::to_string(vMatrix._31) + " " + std::to_string(vMatrix._32) + " " + std::to_string(vMatrix._33) + " " + std::to_string(vMatrix._34) + "\n";
	strMatrix += std::to_string(vMatrix._41) + " " + std::to_string(vMatrix._42) + " " + std::to_string(vMatrix._43) + " " + std::to_string(vMatrix._44) + "\n";
	return strMatrix;
}
//
//HRESULT Engine::ReadJsonFile(const _char* strJsonFilePath, json* pOut)
//
//{
//	std::ifstream input_file(strJsonFilePath);
//
//
//	if (!input_file.is_open()) {
//		// Handle error
//		return E_FAIL;
//	}
//	// JSON 
//	input_file >> *pOut;
//
//	// 
//	input_file.close();
//	return S_OK;
//}
//
//HRESULT Engine::SaveJsonFile(const _char* strJsonFilePath, const json& pJson)
//{
//	std::ofstream file(strJsonFilePath);
//	if (file.is_open()) {
//		file << pJson.dump(4);
//		file.close();
//
//	}
//	else {
//		return E_FAIL;
//	}
//	return S_OK;
//}
