#include "pch.h"

#include <tchar.h>
#include <Windows.h>
#include <wtsapi32.h>
#include <wincred.h>
#include <Securitybaseapi.h>
#include <combaseapi.h>
#include <sstream>
#include <functional>

#include "UserHelper.hpp"

const std::wstring user::process::GetUserNameByTokenW(const HANDLE& token)
{
  LPVOID information = nullptr;
  DWORD infoLen = 0;
  BOOL tokenResult = GetTokenInformation(token, TokenUser, nullptr, 0, &infoLen);

  PTOKEN_USER pUser = nullptr;
  pUser = (PTOKEN_USER)GlobalAlloc(GPTR, infoLen);
  BOOL tokenInfoResult = GetTokenInformation(token, TokenUser, pUser, infoLen, &infoLen);

  SID_NAME_USE   snuSIDNameUse;
  TCHAR          szUser[MAX_PATH] = { 0 };
  DWORD          dwUserNameLength = MAX_PATH;
  TCHAR          szDomain[MAX_PATH] = { 0 };
  DWORD          dwDomainNameLength = MAX_PATH;

  auto lookupResult = LookupAccountSid(NULL, pUser->User.Sid, szUser, &dwUserNameLength, szDomain, &dwDomainNameLength, &snuSIDNameUse);

  GlobalFree(pUser);

  if (!lookupResult) {
    return L"";
  }

  std::wstringstream name = {};
  name << szDomain << L"\\" << szUser;

  return name.str();
}

const std::wstring user::process::GetCurrentProcessUserW()
{
  HANDLE threadHandle = GetCurrentProcess();
  HANDLE token = nullptr;
  BOOL openResult = OpenProcessToken(threadHandle, TOKEN_READ, &token);

  std::wstring name = GetUserNameByTokenW(token);

  CloseHandle(token);

  return name;
}

const std::wstring user::process::GetCurrentThreadUserW(BOOL isTmpersonating)
{
  HANDLE threadHandle = GetCurrentThread();
  HANDLE token = nullptr;
  BOOL openResult = OpenThreadToken(threadHandle, TOKEN_READ, isTmpersonating, &token);

  std::wstring name = GetUserNameByTokenW(token);

  CloseHandle(token);

  return name;
}

const std::string user::process::GetUserNameByToken(const HANDLE& token)
{
  auto userName = GetUserNameByTokenW(token);
  return common::ws2s(userName);
}

const std::string user::process::GetCurrentProcessUser()
{
  auto userName = GetCurrentProcessUserW();
  return common::ws2s(userName);
}

const std::string user::process::GetCurrentThreadUser(BOOL isTmpersonating)
{
  auto userName = GetCurrentThreadUserW(isTmpersonating);
  return common::ws2s(userName);
}

const BOOL user::impersonate::ImpersonateUserW(const std::wstring& message, const std::wstring& caption, PHANDLE userHandle) {
  CREDUI_INFO info = {};
  info.cbSize = sizeof(CREDUI_INFO);
  info.hwndParent = nullptr;
  info.pszMessageText = message.c_str();
  info.pszCaptionText = caption.c_str();
  info.hbmBanner = nullptr;

  LPVOID outAuthBuffer = nullptr;
  ULONG outBufferSize = 0;
  ULONG outPackage = 0;

  auto result = CredUIPromptForWindowsCredentials(&info, 0, &outPackage, nullptr, 0, &outAuthBuffer, &outBufferSize, nullptr, CREDUIWIN_GENERIC | CREDUIWIN_ENUMERATE_CURRENT_USER);
  if (result) return FALSE;

  DWORD userNameLen = CREDUI_MAX_USERNAME_LENGTH;
  DWORD domainNameLen = CREDUI_MAX_DOMAIN_TARGET_LENGTH;
  DWORD passwordLen = CREDUI_MAX_PASSWORD_LENGTH;

  TCHAR lpUserName[CREDUI_MAX_USERNAME_LENGTH];
  TCHAR lpDomainName[CREDUI_MAX_DOMAIN_TARGET_LENGTH];
  TCHAR lpPassword[CREDUI_MAX_PASSWORD_LENGTH];

  result = CredUnPackAuthenticationBuffer(CRED_PACK_PROTECTED_CREDENTIALS, outAuthBuffer, (DWORD)outBufferSize, lpUserName, &userNameLen, lpDomainName, &domainNameLen, lpPassword, &passwordLen);

  SecureZeroMemory(outAuthBuffer, (size_t)outBufferSize);
  CoTaskMemFree(outAuthBuffer);

  if (!result) return FALSE;

  result = LogonUser(lpUserName, lpDomainName, lpPassword, LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, userHandle);
  if (!result) return FALSE;
  result = ImpersonateLoggedOnUser(*userHandle);
  if (!result) return FALSE;

  return TRUE;
}

const BOOL user::impersonate::ImpersonateUserW(const std::wstring& message, const std::wstring& caption, std::function<void()> callback) {
  HANDLE user = nullptr;
  auto result = ImpersonateUserW(message, caption, &user);
  if (!result) return FALSE;

  callback();

  return CloseHandle(user);
}

const BOOL user::impersonate::ImpersonateUser(const std::string& message, const std::string& caption, PHANDLE userHandle)
{
  auto m = common::s2ws(message);
  auto c = common::s2ws(caption);

  return ImpersonateUserW(m, c, userHandle);
}

const BOOL user::impersonate::ImpersonateUser(const std::string& message, const std::string& caption, std::function<void()> callback)
{
  auto m = common::s2ws(message);
  auto c = common::s2ws(caption);

  return ImpersonateUserW(m, c, callback);
}

