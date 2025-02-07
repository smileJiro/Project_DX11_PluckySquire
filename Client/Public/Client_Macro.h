#pragma once


namespace Client
{
#define NUMVIEWPORTS 1
#define PX_SUPPORT_PVD 1 /* pvd 를 위한 플래그 */




#define MAP_2D_DEFAULT_PATH L"../Bin/MapSaveFiles/2D/"
#define MAP_3D_DEFAULT_PATH L"../Bin/MapSaveFiles/3D/"

#define Uimgr CUI_Manager::GetInstance()
#define SECTION_MGR CSection_Manager::GetInstance()

}