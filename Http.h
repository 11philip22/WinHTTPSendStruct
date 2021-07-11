#pragma once

#include <stdio.h>
#include <Windows.h>
#include <winhttp.h>
#include <lmcons.h>

#pragma comment(lib, "Winhttp.lib")

#define BUFFER_SIZE_1_KB 1024
#define GO_CLEAN {dwRet = GetLastError(); goto lblCleanup;}

typedef struct _TEST_STRUCT {
	DWORD	dwOemId;
	WORD	wProcessorArchitecture;
	BOOL	bIsWindowsServer;
	CHAR	cUsername[UNLEN + 1];
} TEST_STRUCT, * PTEST_STRUCT;

DWORD
PushStruct(
	_In_ HINTERNET hSession,
	_In_ LPCWSTR pswzServerName,
	_In_ LPCWSTR pswzVerb,
	_In_ INTERNET_PORT nServerPort,
	_In_ PTEST_STRUCT pMachineId
);

DWORD
CheckConnection(
	_In_ HINTERNET hSession,
	_In_ LPCWSTR pswzServerName
);
