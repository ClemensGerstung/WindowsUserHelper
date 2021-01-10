#ifndef __USER_HELPER_HPP__
#define __USER_HELPER_HPP__

#include "Common.hpp"

#include <Windows.h>
#include <string>
#include <functional>

namespace user {
  namespace process {
    WUH_API const std::wstring GetUserNameByTokenW(const HANDLE& token);

    WUH_API const std::wstring GetCurrentProcessUserW();

    WUH_API const std::wstring GetCurrentThreadUserW(BOOL isImpersonating = FALSE);

    WUH_API const std::string GetUserNameByToken(const HANDLE& token);

    WUH_API const std::string GetCurrentProcessUser();

    WUH_API const std::string GetCurrentThreadUser(BOOL isImpersonating = FALSE);
  }

  namespace impersonate {
    WUH_API const BOOL ImpersonateUserW(const std::wstring& message, const std::wstring& caption, PHANDLE userHandle);

    WUH_API const BOOL ImpersonateUserW(const std::wstring& message, const std::wstring& caption, std::function<void()> callback);

    WUH_API const BOOL ImpersonateUser(const std::string& message, const std::string& caption, PHANDLE userHandle);

    WUH_API const BOOL ImpersonateUser(const std::string& message, const std::string& caption, std::function<void()> callback);
  }
}

#endif
