#include "server/Server.hpp"
#include "logger/logger.hpp"

#include <iostream>
#include <string>

INITIALIZE_EASYLOGGINGPP

int main(int argc, char* argv[])
{
  	try
  	{
  	    if (argc != 3)
  	    {
  	      std::cerr << "Usage: http_server <address> <port>\n";
  	      std::cerr << "Ex   : http_server 127.0.0.1 8000\n";
  	      return 1;
  	    }
        memo::logger::Init("../log/memo.log");

        memo::Server server(argv[1], argv[2]);
        server.run();
  	}
  	catch (std::exception& e)
  	{
        LOG_ERR("exception:\n" << e.what());
  	}
    LOG_TRC("Server stopped.");
    return 0;
}
