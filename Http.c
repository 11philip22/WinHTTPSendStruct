#include "Http.h"

DWORD
PushStruct(
	_In_ CONST HINTERNET hSession,
	_In_ CONST LPCWSTR pswzServerName,
	_In_ CONST LPCWSTR pswzVerb,
	_In_ CONST INTERNET_PORT nServerPort,
	_In_ CONST PTEST_STRUCT pMachineId																																									// NOLINT(misc-misplaced-const
) {
	DWORD			dwRet = ERROR_SUCCESS;
	DWORD			dwContentLength;
	CONST DWORD		dwBoundaryValue = GetTickCount();
	DWORD			options = SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | SECURITY_FLAG_IGNORE_UNKNOWN_CA;
	BOOL			bResults;
	HINTERNET		hConnect = NULL;
	HINTERNET		hRequest = NULL;
	PCHAR			pFormHeader;
	PCHAR			pFinalBoundary;
	PWCHAR			pContentHeader;
	PUCHAR			pBuf = NULL;
	WCHAR			wszContentLength[256];

	CONST PCHAR		pszFormHeader = "------Boundary%08X\r\nContent-Disposition: form-data; name=\"data\"; filename=\"%08x.dat\"\r\nContent-Type: application/octet-stream\r\n\r\n";						// NOLINT(misc-misplaced-const
	CONST PCHAR		pszFinalBoundary = "\r\n------Boundary%08X--\r\n";																																	// NOLINT(misc-misplaced-const
	CONST PWCHAR	wszContentHeader = L"Content-Type: multipart/form-data; boundary=----Boundary%08X";																									// NOLINT(misc-misplaced-const

	ZeroMemory(wszContentLength, 256);

	pFormHeader = calloc(BUFFER_SIZE_1_KB, 1);
	pFinalBoundary = calloc(BUFFER_SIZE_1_KB, 1);
	pContentHeader = calloc(BUFFER_SIZE_1_KB, 1);
	if (!pFormHeader | !pFinalBoundary | !pContentHeader)
		GO_CLEAN

		sprintf_s(pFormHeader, BUFFER_SIZE_1_KB, pszFormHeader, dwBoundaryValue, dwBoundaryValue);
	sprintf_s(pFinalBoundary, BUFFER_SIZE_1_KB, pszFinalBoundary, dwBoundaryValue);
	wsprintf(pContentHeader, wszContentHeader, dwBoundaryValue);

	hConnect = WinHttpConnect(hSession, pswzServerName, nServerPort, 0);
	if (!hConnect)
		GO_CLEAN

		hRequest = WinHttpOpenRequest(hConnect,
			L"POST",
			pswzVerb,
			NULL, WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			0);
	if (!hRequest)
		GO_CLEAN

		bResults = WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, (LPVOID)&options, sizeof(DWORD));
	if (bResults == FALSE)
		GO_CLEAN

		WinHttpAddRequestHeaders(hRequest, pContentHeader, (ULONG)-1L, WINHTTP_ADDREQ_FLAG_ADD);
	WinHttpAddRequestHeaders(hRequest, L"Connection: close", (ULONG)-1L, WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE);

	dwContentLength = strlen(pFormHeader) + sizeof(TEST_STRUCT) + strlen(pFinalBoundary);
	pBuf = calloc(dwContentLength, 1);
	if (!pBuf)
		GO_CLEAN

		strcat_s((PCHAR)pBuf, dwContentLength, pFormHeader);
	CopyMemory(&pBuf[strlen(pFormHeader)], pMachineId, sizeof(TEST_STRUCT));
	CopyMemory(&pBuf[strlen(pFormHeader) + sizeof(TEST_STRUCT)], pFinalBoundary, strlen(pFinalBoundary));
	wsprintf(wszContentLength, L"Content-Length: %d", dwContentLength);

	bResults = WinHttpSendRequest(hRequest,
		wszContentLength,
		-1,
		pBuf,
		dwContentLength,
		dwContentLength,
		0);
	if (bResults == FALSE)
		dwRet = GetLastError();

lblCleanup:
	if (hConnect)
		WinHttpCloseHandle(hConnect);

	if (hRequest)
		WinHttpCloseHandle(hRequest);

	free(pFormHeader);
	free(pFinalBoundary);
	free(pContentHeader);

	if (pBuf)
		free(pBuf);

	return dwRet;
}

DWORD
CheckConnection(
	_In_ CONST HINTERNET hSession,
	_In_ CONST LPCWSTR pswzServerName
) {
	DWORD		dwRet = ERROR_SUCCESS;
	DWORD		dwStatusCode = 0;
	DWORD		dwSize = sizeof(DWORD);
	BOOL		bResults;
	HINTERNET	hConnect;
	HINTERNET	hRequest = NULL;

	hConnect = WinHttpConnect(hSession, pswzServerName, INTERNET_DEFAULT_HTTPS_PORT, 0);
	if (hConnect == INVALID_HANDLE_VALUE)
		GO_CLEAN

		hRequest = WinHttpOpenRequest(hConnect,
			L"GET",
			NULL,
			NULL,
			WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			WINHTTP_FLAG_SECURE);
	if (hRequest == INVALID_HANDLE_VALUE)
		GO_CLEAN

		bResults = WinHttpSendRequest(hRequest,
			WINHTTP_NO_ADDITIONAL_HEADERS,
			0,
			WINHTTP_NO_REQUEST_DATA,
			0,
			0,
			0);
	if (bResults == FALSE)
		GO_CLEAN

		bResults = WinHttpReceiveResponse(hRequest, NULL);
	if (bResults == FALSE)
		GO_CLEAN

		bResults = WinHttpQueryHeaders(hRequest,
			WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
			WINHTTP_HEADER_NAME_BY_INDEX,
			&dwStatusCode,
			&dwSize,
			WINHTTP_NO_HEADER_INDEX);
	if (bResults == FALSE)
		GO_CLEAN

		if (dwStatusCode != 200)
			dwRet = 1L;

lblCleanup:
	if (hConnect)
		WinHttpCloseHandle(hConnect);

	if (hRequest)
		WinHttpCloseHandle(hRequest);

	return dwRet;
}
