#ifndef __WINDOWS_USER_HELPER_H__
#define __WINDOWS_USER_HELPER_H__

#ifdef WINDOWSUSERHELPER_EXPORTS
#define WUH_API    __declspec(dllexport)
#else
#define WUH_API    __declspec(dllimport)
#endif

#include <Windows.h>
#include <wincred.h>
#include <Securitybaseapi.h>
#include <stdint.h>

#define MAX_NAME_LENGTH 255

#if __cplusplus
extern "C" {
#endif

  namespace wuh {
    typedef enum class _ConnectionState {
      Active,
      Connected,
      ConnectQuery,
      Shadow,
      Disconnected,
      Idle,
      Listen,
      Reset,
      Down,
      Init
    } ConnectionState;

    typedef struct WUH_API _UserInformation {
      char userName[MAX_NAME_LENGTH];
      char domainName[MAX_NAME_LENGTH];
      DWORD userNameLength;
      DWORD domainNameLength;
    } UserInformation;

    typedef struct WUH_API _UserImpersonation {
      const char* message;
      const char* caption;
      const char* userName;
      HWND parentWindow;
      HBITMAP bitmap;
    } UserImpersonation;

    typedef struct WUH_API _Server {
      HANDLE handle;
      const char* name;
    } Server;

    typedef struct WUH_API _Session {
      ConnectionState state;
      uint32_t sessionId;
      char sessionName[255];
      char hostName[255];
      char userName[255];
      char domainName[255];
    } Session;

    typedef struct WUH_API _Process {
      uint32_t sessionId;
      uint32_t processId;
      char processName[255];
      uint32_t numberOfThreads;
      uint32_t pagefileUsage;
      uint32_t workingSetSize;
    } Process;

    WUH_API void QueryUserNameByToken(const HANDLE& token, UserInformation* pUserInformation);

    WUH_API void GetCurrentProcessUser(UserInformation* pUserInformation);

    WUH_API void GetCurrentThreadUser(UserInformation* pUserInformation, BOOL isImpersonating = FALSE);

    WUH_API void ImpersonateUser(const UserImpersonation& info, PHANDLE userHandle);

    WUH_API void RunAsImpersonate(const UserImpersonation& info, void (*callback)());

    WUH_API void OpenConnection(const char* name, Server* server);

    WUH_API void CloseConnection(const Server& server);

    WUH_API void EnumerateSessions(const Server& server, uint32_t* pSessionsCount, Session* pSessions);

    WUH_API void EnumerateProcesses(const Server& server, const Session* pSession, uint32_t* pProcessesCount, Process* pProcesses);

    WUH_API void LogoffSession(const Server& server, const Session& session);

    WUH_API void TerminateRemoteProcess(const Server& server, const Process& process);

    WUH_API void UpdateProcessInformation(const Server& server, Process* process);
  }

#if __cplusplus
}
#endif

#endif