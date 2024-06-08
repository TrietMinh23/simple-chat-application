#include "./client.h"

int main(int argc, char *argv[]) {
  cout << ALT_SCREEN_ON;
  const std::string host = (argc > 1) ? argv[1] :  "127.0.0.1";
  const int port = (argc > 2) ? std::atoi(argv[2]) : 8000;
  StrPair credentials = getCredentials();

  cout << ALT_SCREEN_OFF;
  return 0;
}
