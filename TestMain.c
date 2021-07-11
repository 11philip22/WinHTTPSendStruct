#include "Http.h"

INT main() {
	CONST PTEST_STRUCT pTestStruct = malloc(sizeof(TEST_STRUCT));																			// NOLINT(misc-misplaced-const)

	pTestStruct->dwOemId = 4096;
	pTestStruct->wProcessorArchitecture = 24067;
	pTestStruct->bIsWindowsServer = FALSE;

	CHAR cUsername[UNLEN + 1];
	DWORD sdwUsername = UNLEN + 1;
	GetUserNameA(cUsername, &sdwUsername);
	CopyMemory(&pTestStruct->cUsername, cUsername, UNLEN + 1);

	HINTERNET hSession = WinHttpOpen(L"Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0)",  // Internet Explorer 8
		WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS,
		0);
	
	PushStruct(hSession,
		L"localhost",
		L"/signing/",
		6000,
		pTestStruct);
}