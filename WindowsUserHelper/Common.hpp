#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#ifdef WINDOWSUSERHELPER_EXPORTS
#define WUH_API    __declspec(dllexport)
#else
#define WUH_API    __declspec(dllimport)
#endif

#include <locale>
#include <codecvt>

namespace common {
  std::wstring s2ws(const std::string& str)
  {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(str);
  }

  std::string ws2s(const std::wstring& wstr)
  {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
  }
}

#endif
