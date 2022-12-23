#include "Critical_section.h"

Critical_section* Critical_section::ptr = NULL;

CRITICAL_SECTION cs;

Critical_section::Critical_section(void)
{
	ptr = this;
}

Critical_section::~Critical_section(void)
{
	DeleteCriticalSection(&cs);
}

void Critical_section::Cls_OnClose(HWND hwnd)
{
	EndDialog(hwnd, 0);
}

BOOL Critical_section::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	InitializeCriticalSection(&cs);
	return TRUE;
}

void MessageAboutError(DWORD dwError)
{
	LPVOID lpMsgBuf = NULL;
	TCHAR szBuf[300];

	BOOL fOK = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	if (lpMsgBuf != NULL)
	{
		wsprintf(szBuf, TEXT("Îøèáêà %d: %s"), dwError, lpMsgBuf);
		MessageBox(0, szBuf, TEXT("Ñîîáùåíèå îá îøèáêå"), MB_OK | MB_ICONSTOP);
		LocalFree(lpMsgBuf);
	}
}


DWORD WINAPI WriteToFiles(LPVOID lp)
{
	EnterCriticalSection(&cs);
	TCHAR file_name[50];
	for (int i = 0; i <= 4; i++)
	{
		wsprintf(file_name, TEXT("Copy:%d.txt"), i);
		fstream in(TEXT("File.txt"));
		fstream out(file_name, ios_base::out, ios::trunc);
		int buffer[6];
		if (!in.is_open()) {
			MessageAboutError(GetLastError());
			return 1;
		}
		for (int i = 0; i < 6; i++)
		{
			in >> buffer[i];
			out << buffer[i] << " ";
		}
		in.close();
		out.close();
	}
	LeaveCriticalSection(&cs);
	MessageBox(0, TEXT("Copy done!"), TEXT("Copy"), MB_OK);
	return 0;
}

DWORD WINAPI ReadFromFiles(LPVOID lp)
{
	EnterCriticalSection(&cs);
	TCHAR file_name[50];
	for (int i = 0; i <= 4; i++)
	{
		wsprintf(file_name, TEXT("Copy:%d.txt"), i);
		fstream in(TEXT("File.txt"));
		fstream out(TEXT("Result.txt"), ios_base::app);
		int buffer[6];
		if (!in.is_open()) {
			MessageAboutError(GetLastError());
			return 1;
		}
		for (int i = 0; i < 6; i++)
		{
			in >> buffer[i];
			out << buffer[i] << " ";
		}
		in.close();
		out.close();
	}
	LeaveCriticalSection(&cs);
	MessageBox(0, TEXT("Read done!"), TEXT("Read"), MB_OK);
	return 0;
}


void Critical_section::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	if (id == IDC_BUTTON1)
	{
		HANDLE Thread1 = CreateThread(NULL, 0, WriteToFiles, 0, 0, NULL);
		CloseHandle(Thread1);
		HANDLE Thread2 = CreateThread(NULL, 0, ReadFromFiles, 0, 0, NULL);
		CloseHandle(Thread2);
	}
}

BOOL CALLBACK Critical_section::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
	}
	return FALSE;
}