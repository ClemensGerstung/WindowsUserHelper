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

  const std::vector<Session> GetSessions() {
    std::vector<Session> sessions = {};

    PWTS_SESSION_INFO wtsSessions = nullptr;
    DWORD sessionCount = 0;

    auto result = WTSEnumerateSessions(_handle, 0, 1, &wtsSessions, &sessionCount);

    DWORD size = 0;
    LPSTR lpUserName = nullptr;

    for (size_t i = 0; i < sessionCount; i++)
    {
      PWTS_SESSION_INFO wtsSession = (wtsSessions + i);
      result = WTSQuerySessionInformation(_handle, wtsSession->SessionId, WTSUserName, (LPWSTR*)&lpUserName, &size);

      if (result != 0 && size > 1)
      {
        // add item
      }
    }


    return sessions;
  }

  const std::vector<Process> GetProcesses() {
    std::vector<Process> processes = {};


    return processes;
  }
};

class Session {
private:
  HANDLE _server;
  std::string _userName;
  std::string _domain;
  std::string _clientName;
  std::string _sessionName;
  DWORD _sessionId;
  SessionState _state;
public:
  Session(HANDLE server, DWORD sessionId) : _server(server), _sessionId(sessionId) {
    DWORD size = 0;
    WTSQuerySessionInformation(_server, _sessionId, WTSConnectState, (LPWSTR*)&_state, &size);
  }

  const std::vector<Process> GetProcesses() {
    std::vector<Process> processes = {};


    return processes;
  }

  void Logoff() {
    WTSLogoffSession(_server, _sessionId, TRUE);
  }

  void Disconnect() {
    WTSDisconnectSession(_server, _sessionId, TRUE);
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