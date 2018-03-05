// BVHrenderDll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <iostream>
#include "BVHPlayer.h"

using namespace std;

CBVHRender* BVHRender(nullptr);

/*
	Simple check if library starts from allowed (Kstudio's) directory
*/
static BOOL isAllowRender;
static wstring moduleFileName(L"");

wstring getModuleFileName()
{
	MEMORY_BASIC_INFORMATION mbi;
	static int dummy;
	WCHAR szFileName[MAX_PATH];
	VirtualQuery(&dummy, &mbi, sizeof(mbi));
	HMODULE hMod = HMODULE(mbi.AllocationBase);
	GetModuleFileName(hMod, szFileName, sizeof(szFileName));

	wstring moduleFilename(szFileName);
	return moduleFilename;
}


extern "C" __declspec(dllexport)
const HBITMAP * GetHbitmapFrom(int& frames_amount, /*int frame_number,*/
	int frame_Width, int frame_Hight,
	HDC hScreenDC, LPSTR file_name)
{

#if !DEBUG
	if (moduleFileName.empty())
	{
		moduleFileName = getModuleFileName();
		const wstring allowName1(L"Kstudio");
		const wstring allowName2(L"TrackScripts");
		const wstring allowName3(L"For Krammer");
		isAllowRender = (moduleFileName.find(allowName2) != string::npos || moduleFileName.find(allowName1) != string::npos || moduleFileName.find(allowName3) != string::npos);
	}

	if (!isAllowRender)
		return nullptr;
#endif
	if (!BVHRender)
	{
		BVHRender = new CBVHRender();
	}

	const auto result = BVHRender->GetBitmap(frames_amount, frame_Width, frame_Hight, hScreenDC, file_name);

	if (result == nullptr)
	{
		frames_amount = 0;
	}

	return result;
}

extern "C" __declspec(dllexport)
void FreeMemory()
{
	if (BVHRender)
	{
		BVHRender->FreeMemory();
		delete BVHRender;
		BVHRender = nullptr;
	}
}

extern "C" __declspec(dllexport)
int* GetErrInfo(int& numb)
{
	if (BVHRender)
	{
		numb = BVHRender->GetNumbErr();
		return BVHRender->GetPtrErr();
	}
	numb = 0;
	return nullptr;
}

extern "C" __declspec(dllexport)
int GetFrameNumb()
{
	if (BVHRender)
		return BVHRender->GetNumFrame();
	else
		return 0;
}

extern "C" __declspec(dllexport)
float GetInterval()
{
	if (BVHRender)
		return BVHRender->GetInterval();
	else
		return 0.0f;
}

#if TEST

int main()
{


	HBITMAP bitmap(nullptr);
	//CPlayer* player(nullptr);
	LPSTR file_name("..\\..\\bvh files\\13-down zigzag.bvh");
	int win_widh(512);
	int win_hith(512);
	int numb_frame(5);
	int error(0);
	int numb_err(0);
	//player = new CPlayer();
	/*player->GlutInit(); */
	//Пример использования функции GetHbitmapFrom
	////////////////////////////////////////////////////////////////////////////////////////



	cout << "Press any key" << endl;
	//getchar();

	//FreeAllMemory();

	cout << "Press any key for exit" << endl;
	//getchar();

	return 0;
}

#endif