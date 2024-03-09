#include "pch.h"
#include "ComObject.h"

MintChoco::cComLibs* MintChoco::cComLibs::pComLibs = nullptr;

bool MintChoco::cComLibs::Init() {
	if (!pComLibs) {
		pComLibs = new cComLibs();
		if (pComLibs) {
			if (!pComLibs->Create()) {
				Deinit();
			}
		}
	}

	return pComLibs;
}

void MintChoco::cComLibs::Deinit() {
	Destroy();
}

MintChoco::cComLibs* MintChoco::cComLibs::GetInstance() {
	return pComLibs;
}

MintChoco::cComLibs::cComLibs() {

}

MintChoco::cComLibs::~cComLibs() {

}

bool MintChoco::cComLibs::Create() {
	return CoInitialize(NULL) == S_OK;
}

void MintChoco::cComLibs::Destroy() {
	if (pComLibs)
		delete pComLibs;
	pComLibs = nullptr;
}
