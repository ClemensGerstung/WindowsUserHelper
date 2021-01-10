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

public:
  Server() : _handle(WTS_CURRENT_SERVER_HANDLE)
  {
  }

  Server(const std::string& server) {
    _handle = WTSOpenServer((LPWSTR)server.c_str());
  }

  Server(const char* server) {
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
  Server* _server;
  std::wstring _userName;
  std::wstring _domain;
  std::wstring _clientName;
  std::wstring _sessionName;
  DWORD _sessionId;
  SessionState _state;
public:
  Session(Server* server, DWORD sessionId) : _server(server), _sessionId(sessionId) {
    DWORD size = 0;
    LPWSTR lpState = nullptr;
    LPWSTR lpUserName = nullptr;
    LPWSTR lpDomainName = nullptr;
    LPWSTR lpClientName = nullptr;
    LPWSTR lpSessionName = nullptr;

    WTSQuerySessionInformation(_server->Handle(), _sessionId, WTSConnectState, &lpState, &size);
    WTSQuerySessionInformation(_server->Handle(), _sessionId, WTSUserName, &lpUserName, &size);
    WTSQuerySessionInformation(_server->Handle(), _sessionId, WTSDomainName, &lpDomainName, &size);
    WTSQuerySessionInformation(_server->Handle(), _sessionId, WTSClientName, &lpClientName, &size);
    WTSQuerySessionInformation(_server->Handle(), _sessionId, WTSWinStationName, &lpSessionName, &size);

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

  const std::vector<Process> GetProcesses() {
    std::vector<Process> processes = {};


    return processes;
  }

  void Logoff() {
    WTSLogoffSession(_server->Handle(), _sessionId, TRUE);
  }

  void Disconnect() {
    WTSDisconnectSession(_server->Handle(), _sessionId, TRUE);
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
  HANDLE _server;
  DWORD _processId;

public:
  Process(HANDLE server, DWORD session, DWORD processId) {
  }

  void Terminate() {
    WTSTerminateProcess(_server, _processId, -1);
  }
};


int main() {
  //std::wcout << "Current Process User: " << user::process::GetCurrentProcessUser() << std::endl;
  //user::impersonate::ImpersonateUser(L"Please enter credentials of user to impersonate", L"Impersonate Test", []() -> void {
  //  std::wcout << "Impersonate: " << user::process::GetCurrentThreadUser(TRUE) << std::endl;
  //  });

  Server server = {  };
  auto sessions = server.GetSessions();

  for (auto&& session : sessions)
  {
    std::wcout << session.SessionId() << "\t" << session.SessionName() << "\t" << session.Domain() << "\\" << session.UserName() << std::endl;
  }

  auto processes = server.GetProcesses();


  //PWTS_PROCESS_INFO_EX processes = nullptr;
  //DWORD processCount = 0;
  //DWORD level = 1;

  //auto result = WTSEnumerateProcessesEx(WTS_CURRENT_SERVER_HANDLE, &level, WTS_ANY_SESSION, (LPSTR*)&processes, &processCount);

  //std::vector<WTS_PROCESS_INFO_EX> processList = {};
  //processList.resize(processCount);

  //memcpy(processList.data(), processes, processCount * sizeof(WTS_PROCESS_INFO_EX));

  //for (int i = 0; i < processCount; i++)
  //{
  //  PWTS_PROCESS_INFO_EX process = (processes + i);
  //  std::cout << process->pProcessName << std::endl;
  //}

  //result = WTSFreeMemoryEx(WTSTypeProcessInfoLevel1, processes, processCount);

  //typedef DWORD ProcessId;
  //typedef std::string ProcessName;

  //std::map<ProcessId, ProcessName> processesMap = {};

  //PWTS_PROCESS_INFO processes = nullptr;
  //DWORD processCount = 0;
  //auto result = WTSEnumerateProcesses(WTS_CURRENT_SERVER_HANDLE, 0, 1, &processes, &processCount);
  //for (int i = 0; i < processCount; i++)
  //{
  //  PWTS_PROCESS_INFO process = (processes + i);
  //  processesMap.emplace(process->ProcessId, ProcessName(process->pProcessName));
  //}

  //WTSFreeMemory(processes);

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
      sessions.emplace_back(this, wtsSession->SessionId);
    }

    WTSFreeMemory(&lpUserName);
  }

  return sessions;
}
