#include <Windows.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

#include "App.h"

int main(int, char**) {
	cMintScenePlayerApp& App = GetApp();

	App.Create();
	App.Run();
	App.Destroy();
}