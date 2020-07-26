#include "memo/Server.hpp"
#include "logger/logger.hpp"

#include <iostream>
#include <string>

INITIALIZE_EASYLOGGINGPP

int main(int argc, char* argv[])
{
  	try
  	{
  	    if (argc != 4)
  	    {
  	      std::cerr << "Usage: http_server <address> <port> <doc_root>\n";
  	      std::cerr << "  For IPv4, try:\n";
  	      std::cerr << "    receiver 0.0.0.0 80 .\n";
  	      std::cerr << "  For IPv6, try:\n";
  	      std::cerr << "    receiver 0::0 80 .\n";
  	      return 1;
  	    }
        memo::logger::Init("../log/memo.log");

        memo::Server aServer(argv[1], argv[2], argv[3]);
        LOG_TRC("Launching server...");
  	    aServer.run();
  	}
  	catch (std::exception& e)
  	{
        LOG_ERR("exception:\n" << e.what());
  	}
    LOG_TRC("Server stopped.");
    return 0;
}
