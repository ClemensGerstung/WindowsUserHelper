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

  

  wuh::Session session = sessions[1];
  uint32_t processCount = 0;
  wuh::EnumerateProcesses(server, &session, &processCount, nullptr);

  std::vector<wuh::Process> processes = {};
  processes.resize(processCount);

  wuh::EnumerateProcesses(server, &session, &processCount, processes.data());

  wuh::CloseConnection(server);

  std::cin.get();
}