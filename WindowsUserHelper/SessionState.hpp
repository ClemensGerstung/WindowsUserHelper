#ifndef __SESSION_STATE_HPP__
#define __SESSION_STATE_HPP__

#include "pch.h"

#include <WtsApi32.h>

namespace wuh {
  enum class WUH_API SessionState {
    Active = WTSActive,
    Connected = WTSConnected,
    ConnectQuery = WTSConnectQuery,
    Shadow = WTSShadow,
    Disconnected = WTSDisconnected,
    Idle = WTSIdle,
    Listen = WTSListen,
    Reset = WTSReset,
    Down = WTSDown,
    Init = WTSInit
  };
}

#endif