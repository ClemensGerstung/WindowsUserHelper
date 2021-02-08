#include "pch.h"
#include <cstring>

#include <Windows.h>
#include <Securitybaseapi.h>
#include <string>
#include <stdlib.h>
#include <sstream>
#include <combaseapi.h>
#include <wtsapi32.h>
#include <map>

#include "WindowsUserHelper.h"

inline void wuh::QueryUserNameByToken(const HANDLE& token, UserInformation* pUserInformation)
{
  LPVOID information = nullptr;
  DWORD infoLen = 0;
  BOOL tokenResult = GetTokenInformation(token, TokenUser, nullptr, 0, &infoLen);

  PTOKEN_USER pUser = nullptr;
  pUser = (PTOKEN_USER)GlobalAlloc(GPTR, infoLen);
  BOOL tokenInfoResult = GetTokenInformation(token, TokenUser, pUser, infoLen, &infoLen);

  SID_NAME_USE   sid;
  *pUserInformation = {};
  pUserInformation->domainNameLength = MAX_NAME_LENGTH;
  pUserInformation->userNameLength = MAX_NAME_LENGTH;

  if (pUser != nullptr) {
    auto lookupResult = LookupAccountSidA(NULL, pUser->User.Sid, pUserInformation->userName, &(pUserInformation->userNameLength), pUserInformation->domainName, &(pUserInformation->domainNameLength), &sid);

    GlobalFree(pUser);
  }
}

inline void wuh::GetCurrentProcessUser(UserInformation* pUserInformation)
{
  HANDLE threadHandle = GetCurrentProcess();
  HANDLE token = nullptr;
  BOOL openResult = OpenProcessToken(threadHandle, TOKEN_READ, &token);

  QueryUserNameByToken(token, pUserInformation);

  CloseHandle(token);
}

inline void wuh::GetCurrentThreadUser(UserInformation* pUserInformation, BOOL isImpersonating)
{
  HANDLE threadHandle = GetCurrentThread();
  HANDLE token = nullptr;
  BOOL openResult = OpenThreadToken(threadHandle, TOKEN_READ, isImpersonating, &token);

  if (openResult) {
    QueryUserNameByToken(token, pUserInformation);

    CloseHandle(token);
  }
}

inline void wuh::ImpersonateUser(const UserImpersonation& info, PHANDLE userHandle)
{
  std::wstring message(strlen(info.message) + 1, L'\0');
  size_t size = 0;
  mbstowcs_s(&size, &message[0], strlen(info.message) + 1, info.message, _TRUNCATE);

  std::wstring caption(strlen(info.caption) + 1, L'\0');
  mbstowcs_s(&size, &caption[0], strlen(info.caption) + 1, info.caption, _TRUNCATE);

  std::wstring userName(strlen(info.userName) + 1, L'\0');
  mbstowcs_s(&size, &userName[0], strlen(info.userName) + 1, info.userName, _TRUNCATE);

  std::wstring password(0, L'\0');

  CREDUI_INFOW credUiInfo = {};
  credUiInfo.cbSize = sizeof(CREDUI_INFOW);
  credUiInfo.hwndParent = info.parentWindow;
  credUiInfo.pszMessageText = message.c_str();
  credUiInfo.pszCaptionText = caption.c_str();
  credUiInfo.hbmBanner = info.bitmap;

  PBYTE inAuthBuffer = nullptr;
  DWORD inAuthBufferSize = 0;
  LPVOID outAuthBuffer = nullptr;
  ULONG outBufferSize = 0;
  ULONG authPackage = 0;

  BOOL result;

  DWORD userNameLen = CREDUI_MAX_USERNAME_LENGTH;
  DWORD domainNameLen = CREDUI_MAX_DOMAIN_TARGET_LENGTH;
  DWORD passwordLen = CREDUI_MAX_PASSWORD_LENGTH;

  wchar_t lpUserName[CREDUI_MAX_USERNAME_LENGTH];
  wchar_t lpDomainName[CREDUI_MAX_DOMAIN_TARGET_LENGTH];
  wchar_t lpPassword[CREDUI_MAX_PASSWORD_LENGTH];

  if (info.userName != nullptr)
  {
    result = CredPackAuthenticationBufferW(CRED_PACK_GENERIC_CREDENTIALS, (LPWSTR)userName.c_str(), (LPWSTR)password.c_str(), inAuthBuffer, &inAuthBufferSize);

    inAuthBuffer = (BYTE*)HeapAlloc(GetProcessHeap(), 0, inAuthBufferSize);
    result = CredPackAuthenticationBufferW(CRED_PACK_GENERIC_CREDENTIALS, (LPWSTR)userName.c_str(), (LPWSTR)password.c_str(), inAuthBuffer, &inAuthBufferSize);
  }

  result = CredUIPromptForWindowsCredentialsW(&credUiInfo, 0, &authPackage, inAuthBuffer, inAuthBufferSize, &outAuthBuffer, &outBufferSize, nullptr, CREDUIWIN_GENERIC);

  result = CredUnPackAuthenticationBufferW(CRED_PACK_PROTECTED_CREDENTIALS, outAuthBuffer, (DWORD)outBufferSize, lpUserName, &userNameLen, lpDomainName, &domainNameLen, lpPassword, &passwordLen);

  if (inAuthBuffer != nullptr)
  {
    HeapFree(GetProcessHeap(), 0, inAuthBuffer);
  }

  SecureZeroMemory(outAuthBuffer, (size_t)outBufferSize);
  CoTaskMemFree(outAuthBuffer);

  result = LogonUserW(lpUserName, lpDomainName, lpPassword, LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, userHandle);
  result = ImpersonateLoggedOnUser(*userHandle);
}

inline void wuh::RunAsImpersonate(const UserImpersonation& info, void (*callback)())
{
  HANDLE user = nullptr;
  ImpersonateUser(info, &user);
  callback();
  CloseHandle(user);
}

inline void wuh::OpenConnection(const char* name, Server* server) {
  HANDLE handle = WTSOpenServerA((LPSTR)name);
  if (handle != nullptr) {
    *server = { };

    server->name = name;
    server->handle = handle;
  }
  else {
    server = nullptr;
  }
}

inline void wuh::CloseConnection(const Server& server) {
  WTSCloseServer(server.handle);
}

inline void wuh::EnumerateSessions(const Server& server, uint32_t* pSessionsCount, Session* pSessions) {
  PWTS_SESSION_INFO_1A wtsSessions = nullptr;
  DWORD sessionCount = 0;
  DWORD level = 1;

  auto enumResult = WTSEnumerateSessionsExA(server.handle, &level, 1, &wtsSessions, &sessionCount);

  auto copy = [](char* target, LPSTR src) {
    if (src != nullptr) {
      strcpy_s(target, 255, src);
    }
  };

  if (enumResult) {
    if (*pSessionsCount == 0 && pSessions == nullptr) {
      *pSessionsCount = sessionCount;
    }
    else {
      for (size_t i = 0; i < sessionCount; i++) {
        PWTS_SESSION_INFO_1A wtsSession = (wtsSessions + i);
        Session* session = (pSessions + i);

        session->state = (ConnectionState)wtsSession->State;
        session->sessionId = wtsSession->SessionId;

        copy(session->sessionName, wtsSession->pSessionName);
        copy(session->hostName, wtsSession->pHostName);
        copy(session->userName, wtsSession->pUserName);
        copy(session->domainName, wtsSession->pDomainName);
      }
    }

    if (wtsSessions != nullptr) {
      BOOL result = WTSFreeMemoryExA(WTSTypeSessionInfoLevel1, &wtsSessions, sessionCount);
    }
  }
}

inline void wuh::EnumerateProcesses(const Server& server, const Session* pSession, uint32_t* pProcessesCount, Process* pProcesses)
{
  DWORD sessionId = WTS_ANY_SESSION;
  if (pSession != nullptr) sessionId = pSession->sessionId;

  WTS_PROCESS_INFOA* pProcessInfo = nullptr;
  WTS_PROCESS_INFO_EXA* pProcessInfoEx = nullptr;
  DWORD level = 1;
  DWORD processCount = 0;

  auto copy = [](char* target, LPSTR src) {
    if (src != nullptr) {
      strcpy_s(target, 255, src);
    }
  };

  auto result = WTSEnumerateProcessesExA(server.handle, &level, sessionId, (LPSTR*)&pProcessInfoEx, &processCount);

  if (result) {
    if (pProcesses == nullptr && *pProcessesCount == 0) {
      *pProcessesCount = processCount;
    }
    else if (pProcesses != nullptr && *pProcessesCount > 0) {
      processCount = 0;

      result = WTSEnumerateProcessesA(server.handle, 0, 1, &pProcessInfo, &processCount);
      
      if (result) {
        std::map<DWORD, WTS_PROCESS_INFOA> procs = {};
        for (DWORD i = 0; i < processCount; i++) {
          WTS_PROCESS_INFOA processInfo = *(pProcessInfo + i);
          procs.emplace(processInfo.ProcessId, processInfo);
        }

        for (uint32_t i = 0; i < *pProcessesCount; i++) {
          WTS_PROCESS_INFO_EXA* processInfoEx = (pProcessInfoEx + i);
          Process* process = (pProcesses + i);

          copy(process->processName, procs[processInfoEx->ProcessId].pProcessName);
          process->numberOfThreads = processInfoEx->NumberOfThreads;
          process->pagefileUsage = processInfoEx->PagefileUsage;
          process->processId = processInfoEx->ProcessId;
          process->sessionId = processInfoEx->SessionId;
          process->workingSetSize = processInfoEx->WorkingSetSize;
        }

        WTSFreeMemory(pProcessInfo);
      }
    }

    result = WTSFreeMemoryExA(WTSTypeProcessInfoLevel1, pProcessInfoEx, *pProcessesCount);
  }
}

inline void wuh::LogoffSession(const Server& server, const Session& session) {
  auto result = WTSLogoffSession(server.handle, session.sessionId, TRUE);
}

inline void wuh::TerminateRemoteProcess(const Server& server, const Process& process) {
  auto result = WTSTerminateProcess(server.handle, process.processId, -1);
}

inline void wuh::UpdateProcessInformation(const Server& server, Process* process) {
  WTS_PROCESS_INFO_EXA* pProcessInfoEx = nullptr;
  DWORD level = 1;
  DWORD processCount = 0;
  
  auto result = WTSEnumerateProcessesExA(server.handle, &level, process->sessionId, (LPSTR*)&pProcessInfoEx, &processCount);
  if (result) {
    for (uint32_t i = 0; i < processCount; i++) {
      WTS_PROCESS_INFO_EXA* processInfoEx = (pProcessInfoEx + i);
      if (processInfoEx->ProcessId != process->processId) continue;

      process->numberOfThreads = processInfoEx->NumberOfThreads;
      process->pagefileUsage = processInfoEx->PagefileUsage;
      process->workingSetSize = processInfoEx->WorkingSetSize;

      break;
    }

    result = WTSFreeMemoryExA(WTSTypeProcessInfoLevel1, pProcessInfoEx, processCount);
  }
}