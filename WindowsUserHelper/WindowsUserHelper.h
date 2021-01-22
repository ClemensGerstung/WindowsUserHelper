#ifndef __WINDOWS_USER_HELPER_H__
#define __WINDOWS_USER_HELPER_H__

#ifdef WINDOWSUSERHELPER_EXPORTS
#define WUH_API    __declspec(dllexport)
#else
#define WUH_API    __declspec(dllimport)
#endif

#include <Windows.h>
#include <stdint.h>

#if __cplusplus
extern "C" {
#endif

  namespace wuh {
#define MAX_NAME_LENGTH 255

    typedef struct WUH_API _UserInformation {
      char userName[MAX_NAME_LENGTH];
      char domainName[MAX_NAME_LENGTH];
      DWORD userNameLength;
      DWORD domainNameLength;
    } UserInformation;

    WUH_API void QueryUserNameByToken(const HANDLE& token, UserInformation* pUserInformation);

    WUH_API void GetCurrentProcessUser(UserInformation* pUserInformation);

    WUH_API void GetCurrentThreadUser(UserInformation* pUserInformation, BOOL isImpersonating = FALSE);
  }

#if __cplusplus
}
#endif

#endif