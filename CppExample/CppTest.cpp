#include <iostream>
#include <memory>
#include <vector>
#include <tchar.h>
#include <Windows.h>
#include <wtsapi32.h>
#include <wincred.h>
#include <Securitybaseapi.h>
#include <map>
#include <sstream>
#include <functional>
#include <algorithm>
#include <numeric>

#include <WindowsUserHelper.h>

class Server;
class Session;
class Process;

enum class SessionState {
  Active = WTSActive,
  Connected = WTSConnected,
  ConnectQuery = WTSConnectQuery,
  WTSShadow = WTSShadow,
  Disconnected = WTSDisconnected,
  Idle = WTSIdle,
  Listen = WTSListen,
  Reset = WTSReset,
  Down = WTSDown,
  Init = WTSInit
};

class Server {
private:
  HANDLE _handle;
  std::shared_ptr<Server> _this;

public:
  Server() : _handle(WTS_CURRENT_SERVER_HANDLE)
  {
    _this = std::shared_ptr<Server>(this);
  }

  Server(const std::string& server) : Server() {
    _handle = WTSOpenServer((LPWSTR)server.c_str());
  }

  Server(const char* server) : Server() {
    _handle = WTSOpenServer((LPWSTR)server);
  }

  ~Server()
  {
    if (_handle != WTS_CURRENT_SERVER_HANDLE)
      WTSCloseServer(_handle);
  }

  const std::vector<Session> GetSessions();

  const std::vector<Process> GetProcesses() {
    std::vector<Process> processes = {};


    return processes;
  }

  const HANDLE Handle() {
    return _handle;
  }
};

class Session {
private:
  std::weak_ptr<Server> _server;
  std::shared_ptr<Session> _this;
  std::wstring _userName;
  std::wstring _domain;
  std::wstring _clientName;
  std::wstring _sessionName;
  DWORD _sessionId;
  SessionState _state;
public:
  Session(const std::shared_ptr<Server>& server, DWORD sessionId) : _server(server), _sessionId(sessionId) {
    _this = std::shared_ptr<Session>(this);

    DWORD size = 0;
    LPWSTR lpState = nullptr;
    LPWSTR lpUserName = nullptr;
    LPWSTR lpDomainName = nullptr;
    LPWSTR lpClientName = nullptr;
    LPWSTR lpSessionName = nullptr;

    WTSQuerySessionInformation(server->Handle(), _sessionId, WTSConnectState, &lpState, &size);
    WTSQuerySessionInformation(server->Handle(), _sessionId, WTSUserName, &lpUserName, &size);
    WTSQuerySessionInformation(server->Handle(), _sessionId, WTSDomainName, &lpDomainName, &size);
    WTSQuerySessionInformation(server->Handle(), _sessionId, WTSClientName, &lpClientName, &size);
    WTSQuerySessionInformation(server->Handle(), _sessionId, WTSWinStationName, &lpSessionName, &size);

    _userName.assign(lpUserName);
    _domain.assign(lpDomainName);
    _clientName.assign(lpClientName);
    _sessionName.assign(lpSessionName);

    _state = *(reinterpret_cast<::SessionState*>(lpState));

    WTSFreeMemory(&lpSessionName);
    WTSFreeMemory(&lpClientName);
    WTSFreeMemory(&lpDomainName);
    WTSFreeMemory(&lpUserName);
    WTSFreeMemory(&lpState);
  }

  const std::vector<Process> GetProcesses();

  void Logoff() {
    if (auto pServer = _server.lock())
    {
      WTSLogoffSession(pServer->Handle(), _sessionId, TRUE);
    }
  }

  void Disconnect() {
    if (auto pServer = _server.lock())
    {
      WTSDisconnectSession(pServer->Handle(), _sessionId, TRUE);
    }
  }

  const std::wstring& UserName() const {
    return _userName;
  }

  const std::wstring& Domain() const {
    return _domain;
  }

  const std::wstring& ClientName() const {
    return _clientName;
  }

  const std::wstring& SessionName() const {
    return _sessionName;
  }

  const DWORD SessionId() const {
    return _sessionId;
  }

  const SessionState SessionState() const {
    return _state;
  }
};

class Process {
  std::weak_ptr<Server> _server;
  std::weak_ptr<Session> _session;
  std::wstring _processName;
  DWORD _workingSetSize;
  DWORD _processId;
  DWORD _sessionId;

public:
  Process(const std::shared_ptr<Server>& server, DWORD processId, const std::wstring& processName, DWORD workingSetSize, DWORD sessionId)
  {
    _server = server;
    _processId = processId;
    _processName = processName;
    _workingSetSize = workingSetSize;
    _sessionId = sessionId;
  }

  Process(const std::shared_ptr<Server>& server, const std::shared_ptr<Session>& session, DWORD processId, const std::wstring& processName, DWORD workingSetSize) {
    _server = server;
    _session = session;
    _processId = processId;
    _processName = processName;
    _workingSetSize = workingSetSize;
    _sessionId = session->SessionId();
  }

  void Terminate() {
    if (auto pServer = _server.lock()) {
      WTSTerminateProcess(pServer->Handle(), _processId, -1);
    }
  }

  DWORD WorkingSetSize() const {
    return _workingSetSize;
  }
};



int main() {
  /*{
    wuh::UserInformation info = {};
    wuh::GetCurrentProcessUser(&info);

    std::cout << info.domainName << "\\" << info.userName << std::endl;

    HANDLE user = nullptr;
    wuh::UserImpersonation impersonation = { };
    impersonation.message = "Test";
    impersonation.caption = "Blaa";
    impersonation.userName = "Test User";

    wuh::ImpersonateUser(impersonation, &user);

    info = {};
    wuh::GetCurrentThreadUser(&info, TRUE);

    std::cout << info.domainName << "\\" << info.userName << std::endl;

    CloseHandle(user);

    wuh::RunAsImpersonate(impersonation, []() {
      wuh::UserInformation info = {};
      wuh::GetCurrentThreadUser(&info, TRUE);

      std::cout << info.domainName << "\\" << info.userName << std::endl;
      });
  }*/

  wuh::Server server = {};
  wuh::OpenConnection("localhost", &server);

  uint32_t sessionsCount = 0;
  wuh::EnumerateSessions(server, &sessionsCount, nullptr);

  std::vector<wuh::Session> sessions = {};
  sessions.resize(sessionsCount);

  wuh::EnumerateSessions(server, &sessionsCount, sessions.data());

  wuh::CloseConnection(server);

  std::cin.get();
}

inline const std::vector<Session> Server::GetSessions() {
  std::vector<Session> sessions = {};

  PWTS_SESSION_INFO wtsSessions = nullptr;
  DWORD sessionCount = 0;

  auto result = WTSEnumerateSessions(_handle, 0, 1, &wtsSessions, &sessionCount);

  DWORD size = 0;
  LPWSTR lpUserName = nullptr;

  for (size_t i = 0; i < sessionCount; i++)
  {
    PWTS_SESSION_INFO wtsSession = (wtsSessions + i);
    result = WTSQuerySessionInformation(_handle, wtsSession->SessionId, WTSUserName, &lpUserName, &size);

    if (result != 0 && lstrlen(lpUserName) > 0)
    {
      sessions.emplace_back(_this, wtsSession->SessionId);
    }

    WTSFreeMemory(&lpUserName);
  }

  return sessions;
}

inline const std::vector<Process> Session::GetProcesses() {
  std::vector<Process> sessionProcesses = {};
  std::map<DWORD, std::wstring> processesMap = {};

  if (auto pServer = _server.lock())
  {
    {
      PWTS_PROCESS_INFO processes = nullptr;
      DWORD processCount = 0;

      auto result = WTSEnumerateProcesses(pServer->Handle(), 0, 1, &processes, &processCount);
      for (int i = 0; i < processCount; i++)
      {
        PWTS_PROCESS_INFO process = (processes + i);
        processesMap.emplace(process->ProcessId, std::wstring(process->pProcessName));
      }

      WTSFreeMemory(processes);
    }

    {
      PWTS_PROCESS_INFO_EX processes = nullptr;
      DWORD processCount = 0;
      DWORD level = 1;

      auto result = WTSEnumerateProcessesEx(pServer->Handle(), &level, _sessionId, (LPWSTR*)&processes, &processCount);
      for (int i = 0; i < processCount; i++)
      {
        PWTS_PROCESS_INFO_EX process = (processes + i);

        DWORD processId = process->ProcessId;
        std::wstring processName = processesMap[process->ProcessId];
        DWORD workingSetSize = process->WorkingSetSize;

        sessionProcesses.emplace_back(pServer, _this, processId, processName, workingSetSize);
      }

      WTSFreeMemory(processes);
    }
  }

  return sessionProcesses;
}
