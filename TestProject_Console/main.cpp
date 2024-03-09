#include <cassert>
#include <iostream>

#include <Windows.h>
#include "App.h"

#include "ComObject.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	MintChoco::cComLibs::Init();

	cApp App;
	if (!App.Create(hInstance, TEXT("TestAPP"), TEXT("TestAPP"), nCmdShow)) {
		assert(false);
		App.Destroy();
		return 0;
	}

	App.Run();

	App.Destroy();
#ifdef _DEBUG
	//system("pause");
#endif
	return 0;
}
