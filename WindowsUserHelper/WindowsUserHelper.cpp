#include "pch.h"
#include <cstring>

#include <Windows.h>
#include <Securitybaseapi.h>
#include <sstream>
#include <combaseapi.h>

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