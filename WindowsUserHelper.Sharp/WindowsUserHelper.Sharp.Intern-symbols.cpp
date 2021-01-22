#define _LIBCPP_DISABLE_VISIBILITY_ANNOTATIONS
#define _LIBCPP_HIDE_FROM_ABI

#include <WindowsUserHelper.h>
#include <new>

extern "C" { void c__N_wuh_S__UserInformation__UserInformation(void* __instance) { new (__instance) wuh::_UserInformation(); } }
extern "C" { void c__N_wuh_S__UserInformation__UserInformation___1__N_wuh_S__UserInformation(void* __instance, const wuh::_UserInformation& _0) { new (__instance) wuh::_UserInformation(_0); } }
wuh::_UserInformation& (wuh::_UserInformation::*_0)(wuh::_UserInformation&&) = &wuh::_UserInformation::operator=;
