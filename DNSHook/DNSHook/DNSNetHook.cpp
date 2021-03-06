#include "stdafx.h"
#include <MSWSock.h>
#include "SAStatusLog.h"
#include "DataFileMgr.h"
#include ".\WebServer\MiniWebServer.h"

CSAStatusLog g_loger(L"networkservice");

BOOL DecodeDNSResponse( char *pRecvBuf,int nBufLen );

 LPFN_WSARECVMSG pWsaRecvMsg = NULL;

 INT  WINAPI MY_LPFN_WSARECVMSG (
	 IN SOCKET s, 
	 IN OUT LPWSAMSG lpMsg, 
	 __out_opt LPDWORD lpdwNumberOfBytesRecvd, 
	 IN LPWSAOVERLAPPED lpOverlapped, 
	 IN LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
	 )
 {

	
	 int nRet = pWsaRecvMsg(
		 s, 
		 lpMsg, 
		 lpdwNumberOfBytesRecvd, 
		 lpOverlapped, 
		 lpCompletionRoutine
		 );

	 if (lpMsg)
	 {
		 DecodeDNSResponse(lpMsg->lpBuffers->buf,lpMsg->lpBuffers->len);

// 		 CString strMsgOut;
// 		 CString strTemp;
// 		 for ( int i=0;i<lpMsg->lpBuffers->len;i++)
// 		 {
// 			 strTemp.Format(L"%c",lpMsg->lpBuffers->buf[i]);
// 			 strMsgOut+=strTemp;
// 		 }
// 		 OutputDebugStringW(L"Recv "+strMsgOut);
	 }

	 return nRet;
 }

int (WINAPI *pWSAIoctl)(
						__in SOCKET s,
						__in DWORD dwIoControlCode,
						__in LPVOID lpvInBuffer,
						__in DWORD cbInBuffer,
						__out LPVOID lpvOutBuffer,
						__in DWORD cbOutBuffer,
						__out LPDWORD lpcbBytesReturned,
						__in LPWSAOVERLAPPED lpOverlapped,
						__in LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine 
						) = WSAIoctl;
int WINAPI MyWSAIoctl(
					  __in SOCKET s,
					  __in DWORD dwIoControlCode,
					  __in LPVOID lpvInBuffer,
					  __in DWORD cbInBuffer,
					  __out LPVOID lpvOutBuffer,
					  __in DWORD cbOutBuffer,
					  __out LPDWORD lpcbBytesReturned,
					  __in LPWSAOVERLAPPED lpOverlapped,
					  __in LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine 
					  )
{
	int TReturn = pWSAIoctl(
		s,
		dwIoControlCode,
		lpvInBuffer,
		cbInBuffer,
		lpvOutBuffer,
		cbOutBuffer,
		lpcbBytesReturned,
		lpOverlapped,
		lpCompletionRoutine
		);

	static GUID guidRecvMsg = WSAID_WSARECVMSG;
	if ( SIO_GET_EXTENSION_FUNCTION_POINTER == dwIoControlCode && sizeof(guidRecvMsg) == cbInBuffer )
	{
		if (memcmp(&guidRecvMsg,lpvInBuffer,sizeof(guidRecvMsg)) == 0)
		{
			if( sizeof(DWORD_PTR) == cbOutBuffer )
			{
				if( NULL == pWsaRecvMsg )
				{
					pWsaRecvMsg = (LPFN_WSARECVMSG)(*(LPVOID *)lpvOutBuffer);

					DetourTransactionBegin();
					DetourUpdateThread(GetCurrentThread());

					DetourAttach(&(PVOID&)pWsaRecvMsg, (PBYTE)MY_LPFN_WSARECVMSG);
					DetourDetach(&(PVOID&)pWSAIoctl, (PBYTE)MyWSAIoctl);

					DetourTransactionCommit();

				}
			}
		}
	}

	return TReturn;
};


//获取当前模块句柄
HMODULE ModuleHandleByAddr(const void* ptrAddr)  
{  
	MEMORY_BASIC_INFORMATION info;  
	::VirtualQuery(ptrAddr, &info, sizeof(info));  
	return (HMODULE)info.AllocationBase;  
}  
/*  
功能：获取当前模块句柄
返回值：当前模块句柄
*/  
HMODULE ThisModuleHandle()  
{  
	static HMODULE sInstance = ModuleHandleByAddr((void*)&ThisModuleHandle);  
	return sInstance;  
}

VOID  InitDnsHook(  )
{
	g_loger.StatusOut(L"Start Hook");

	BOOL bWebServer = StartWebServer(L"http://www.pjpj1.com/");
	if ( FALSE == bWebServer )
	{
		return ;
	}

	CString strThisModulePath;
	GetModuleFileNameW(ThisModuleHandle(),strThisModulePath.GetBuffer(MAX_PATH),MAX_PATH);
	strThisModulePath.ReleaseBuffer();
	strThisModulePath+=L".cfg";

	StartParseDataFile( strThisModulePath );

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

 	DetourAttach(&(PVOID&)pWSAIoctl, (PBYTE)MyWSAIoctl);

	DetourTransactionCommit();

	g_loger.StatusOut(L"Hook End");
}