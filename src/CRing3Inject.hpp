#ifndef AFX_PROCESS_H__3BA1E2EE_BD39_483B_A298_73FAD53A51BE__INCLUDED_
#define AFX_PROCESS_H__3BA1E2EE_BD39_483B_A298_73FAD53A51BE__INCLUDED_
#ifndef __VOL_BASE_H__
#include <sys/base/libs/win_base/vol_base.h>
#endif
#include <winternl.h>
/**************************************************************\
 * CRing3Inject
 * Ring3下的DLL注入工具
 * NtCreateThreadEx + LdrLoadDll方式实现,可以注入系统进程
\**************************************************************/
class CRing3Inject
{
private:
	typedef NTSTATUS(NTAPI *pfnNtCreateThreadEx)(
		PHANDLE hThread,
		ACCESS_MASK DesiredAccess,
		PVOID ObjectAttributes,
		HANDLE ProcessHandle,
		PVOID lpStartAddress,
		PVOID lpParameter,
		ULONG Flags,
		SIZE_T StackZeroBits,
		SIZE_T SizeOfStackCommit,
		SIZE_T SizeOfStackReserve,
		PVOID lpBytesBuffer);

	typedef struct _LSA_UNICODE_STRING
	{
		USHORT Length;
		USHORT MaximumLength;
		PWSTR Buffer;
	} LSA_UNICODE_STRING, *PLSA_UNICODE_STRING, UNICODE_STRING, *PUNICODE_STRING;

	typedef NTSTATUS(NTAPI *pRtlInitUnicodeString)(PUNICODE_STRING, PCWSTR);
	typedef NTSTATUS(NTAPI *pLdrLoadDll)(PWCHAR, ULONG, PUNICODE_STRING, PHANDLE);

	typedef struct _THREAD_DATA
	{
		pRtlInitUnicodeString fnRtlInitUnicodeString;
		pLdrLoadDll fnLdrLoadDll;
		UNICODE_STRING UnicodeString;
		WCHAR DllName[260];
		PWCHAR DllPath;
		ULONG Flags;
		HANDLE ModuleHandle;
	} THREAD_DATA, *PTHREAD_DATA;

public:
	CRing3Inject()
	{
	}
	~CRing3Inject()
	{
	}
	BOOL InjectDll(DWORD dwProcessId, LPCWSTR lpcwDllPath)
	{
		BOOL bRet = FALSE;
		HANDLE hProcess = NULL, hThread = NULL;
		LPVOID pCode = NULL;
		LPVOID pThreadData = NULL;
		__try
		{
			if (!EnableDebugPrivilege())
				return -1;
			// 打开目标进程;
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, dwProcessId);
			if (hProcess == NULL)
				__leave;
			// 申请空间，把我们的代码和数据写入目标进程空间里;
			THREAD_DATA data;
			HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
			data.fnRtlInitUnicodeString = (pRtlInitUnicodeString)GetProcAddress(hNtdll, "RtlInitUnicodeString");
			data.fnLdrLoadDll = (pLdrLoadDll)GetProcAddress(hNtdll, "LdrLoadDll");
			memcpy(data.DllName, lpcwDllPath, (wcslen(lpcwDllPath) + 1) * sizeof(WCHAR));
			data.DllPath = NULL;
			data.Flags = 0;
			data.ModuleHandle = INVALID_HANDLE_VALUE;
			pThreadData = VirtualAllocEx(hProcess, NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			if (pThreadData == NULL)
				__leave;
			BOOL bWriteOK = WriteProcessMemory(hProcess, pThreadData, &data, sizeof(data), NULL);
			if (!bWriteOK)
				__leave;
			// 写入代码;
			SIZE_T SizeOfCode = (SIZE_T)ThreadProcEnd - (SIZE_T)ThreadProc;
			pCode = VirtualAllocEx(hProcess, NULL, SizeOfCode, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			if (pCode == NULL)
				__leave;
			bWriteOK = WriteProcessMemory(hProcess, pCode, (PVOID)ThreadProc, SizeOfCode, NULL);
			if (!bWriteOK)
				__leave;
			// 创建远程线程,把ThreadProc作为线程起始函数,pThreadData作为参数;
			hThread = MyCreateRemoteThread(hProcess, (LPTHREAD_START_ROUTINE)pCode, pThreadData);
			if (hThread == NULL)
				__leave;
			// 等待完成;
			WaitForSingleObject(hThread, INFINITE);
			bRet = TRUE;
		}
		__finally
		{
			if (pThreadData != NULL)
				VirtualFreeEx(hProcess, pThreadData, 0, MEM_RELEASE);
			if (pCode != NULL)
				VirtualFreeEx(hProcess, pCode, 0, MEM_RELEASE);
			if (hThread != NULL)
				CloseHandle(hThread);
			if (hProcess != NULL)
				CloseHandle(hProcess);
		}
		return bRet;
	}

private:
	static HANDLE __stdcall ThreadProc(PTHREAD_DATA data)
	{
		data->fnRtlInitUnicodeString(&data->UnicodeString, data->DllName);
		data->fnLdrLoadDll(data->DllPath, data->Flags, &data->UnicodeString, &data->ModuleHandle);
		return data->ModuleHandle;
	}
	static DWORD __stdcall ThreadProcEnd()
	{
		return 0;
	}
	// 提升到Debug权限
	BOOL EnableDebugPrivilege()
	{
		HANDLE hToken;
		TOKEN_PRIVILEGES tkp;
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
			return (FALSE);
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
		if (GetLastError() != ERROR_SUCCESS)
			return FALSE;
		return TRUE;
	}
	// 创建远程线程
	HANDLE MyCreateRemoteThread(HANDLE hProcess, LPTHREAD_START_ROUTINE pThreadProc, LPVOID pRemoteBuf)
	{
		HANDLE hThread = NULL;
		pfnNtCreateThreadEx fnNtCreateThreadEx = (pfnNtCreateThreadEx)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtCreateThreadEx");
		if (fnNtCreateThreadEx)
		{
			fnNtCreateThreadEx(&hThread, 0x1FFFFF, NULL, hProcess, pThreadProc, pRemoteBuf,
							   FALSE, NULL, NULL, NULL, NULL);
			if (hThread == NULL)
				return NULL;
		}
		else
		{
			hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);
			if (hThread == NULL)
				return NULL;
		}
		if (WAIT_FAILED == WaitForSingleObject(hThread, INFINITE))
			return NULL;
		return hThread;
	}
}; // CRing3Inject

#endif // AFX_PROCESS_H__3BA1E2EE_BD39_483B_A298_73FAD53A51BE__INCLUDED_
