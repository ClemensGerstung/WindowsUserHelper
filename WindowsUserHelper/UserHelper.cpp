#include "pch.h"
//
//
//#include <wtsapi32.h>
//#include <wincred.h>
//#include <Securitybaseapi.h>
//#include <combaseapi.h>
//#include <sstream>
//#include <string>
//
//inline const std::wstring wuh::implementation::GetUserNameByTokenW(const HANDLE& token)
//{
//  LPVOID information = nullptr;
//  DWORD infoLen = 0;
//  BOOL tokenResult = GetTokenInformation(token, TokenUser, nullptr, 0, &infoLen);
//
//  PTOKEN_USER pUser = nullptr;
//  pUser = (PTOKEN_USER)GlobalAlloc(GPTR, infoLen);
//  BOOL tokenInfoResult = GetTokenInformation(token, TokenUser, pUser, infoLen, &infoLen);
//
//  SID_NAME_USE   snuSIDNameUse;
//  TCHAR          szUser[MAX_PATH] = { 0 };
//  DWORD          dwUserNameLength = MAX_PATH;
//  TCHAR          szDomain[MAX_PATH] = { 0 };
//  DWORD          dwDomainNameLength = MAX_PATH;
//
//  auto lookupResult = LookupAccountSid(NULL, pUser->User.Sid, szUser, &dwUserNameLength, szDomain, &dwDomainNameLength, &snuSIDNameUse);
//
//  GlobalFree(pUser);
//
//  if (!lookupResult) {
//    return L"";
//  }
//
//  std::wstringstream name = {};
//  name << szDomain << L"\\" << szUser;
//
//  return name.str();
//}
//
//inline const std::wstring wuh::implementation::GetCurrentProcessUserW()
//{
//  HANDLE threadHandle = GetCurrentProcess();
//  HANDLE token = nullptr;
//  BOOL openResult = OpenProcessToken(threadHandle, TOKEN_READ, &token);
//
//  std::wstring name = GetUserNameByTokenW(token);
//
//  CloseHandle(token);
//
//  return name;
//}
//
//inline const std::wstring wuh::implementation::GetCurrentThreadUserW(BOOL isTmpersonating)
//{
//  HANDLE threadHandle = GetCurrentThread();
//  HANDLE token = nullptr;
//  BOOL openResult = OpenThreadToken(threadHandle, TOKEN_READ, isTmpersonating, &token);
//
//  std::wstring name = GetUserNameByTokenW(token);
//
//  CloseHandle(token);
//
//  return name;
//}
//
//inline const std::string wuh::implementation::GetUserNameByToken(const HANDLE& token)
//{
//  auto userName = GetUserNameByTokenW(token);
//  return wuh::ws2s(userName);
//}
//
//inline const std::string wuh::implementation::GetCurrentProcessUser()
//{
//  auto userName = GetCurrentProcessUserW();
//  return wuh::ws2s(userName);
//}
//
//inline const std::string wuh::implementation::GetCurrentThreadUser(BOOL isTmpersonating)
//{
//  auto userName = GetCurrentThreadUserW(isTmpersonating);
//  return wuh::ws2s(userName);
//}
//
//inline const BOOL wuh::implementation::ImpersonateUserW(const std::wstring& message, const std::wstring& caption, PHANDLE userHandle) {
//  CREDUI_INFO info = {};
//  info.cbSize = sizeof(CREDUI_INFO);
//  info.hwndParent = nullptr;
//  info.pszMessageText = message.c_str();
//  info.pszCaptionText = caption.c_str();
//  info.hbmBanner = nullptr;
//
//  LPVOID outAuthBuffer = nullptr;
//  ULONG outBufferSize = 0;
//  ULONG outPackage = 0;
//
//  auto result = CredUIPromptForWindowsCredentials(&info, 0, &outPackage, nullptr, 0, &outAuthBuffer, &outBufferSize, nullptr, CREDUIWIN_GENERIC | CREDUIWIN_ENUMERATE_CURRENT_USER);
//  if (result) return FALSE;
//
//  DWORD userNameLen = CREDUI_MAX_USERNAME_LENGTH;
//  DWORD domainNameLen = CREDUI_MAX_DOMAIN_TARGET_LENGTH;
//  DWORD passwordLen = CREDUI_MAX_PASSWORD_LENGTH;
//
//  TCHAR lpUserName[CREDUI_MAX_USERNAME_LENGTH];
//  TCHAR lpDomainName[CREDUI_MAX_DOMAIN_TARGET_LENGTH];
//  TCHAR lpPassword[CREDUI_MAX_PASSWORD_LENGTH];
//
//  result = CredUnPackAuthenticationBuffer(CRED_PACK_PROTECTED_CREDENTIALS, outAuthBuffer, (DWORD)outBufferSize, lpUserName, &userNameLen, lpDomainName, &domainNameLen, lpPassword, &passwordLen);
//
//  SecureZeroMemory(outAuthBuffer, (size_t)outBufferSize);
//  CoTaskMemFree(outAuthBuffer);
//
//  if (!result) return FALSE;
//
//  result = LogonUser(lpUserName, lpDomainName, lpPassword, LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, userHandle);
//  if (!result) return FALSE;
//  result = ImpersonateLoggedOnUser(*userHandle);
//  if (!result) return FALSE;
//
//  return TRUE;
//}
//
//inline const BOOL wuh::implementation::ImpersonateUserW(const std::wstring& message, const std::wstring& caption, std::function<void()> callback) {
//  HANDLE user = nullptr;
//  auto result = ImpersonateUserW(message, caption, &user);
//  if (!result) return FALSE;
//
//  callback();
//
//  return CloseHandle(user);
//}
//
//inline const BOOL wuh::implementation::ImpersonateUser(const std::string& message, const std::string& caption, PHANDLE userHandle)
//{
//  auto m = wuh::s2ws(message);
//  auto c = wuh::s2ws(caption);
//
//  return ImpersonateUserW(m, c, userHandle);
//}
//
//inline const BOOL wuh::implementation::ImpersonateUser(const std::string& message, const std::string& caption, std::function<void()> callback)
//{
//  auto m = wuh::s2ws(message);
//  auto c = wuh::s2ws(caption);
//
//  return ImpersonateUserW(m, c, callback);
//}
//
