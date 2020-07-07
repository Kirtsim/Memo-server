#include "memo/Server.hpp"

#include <iostream>
#include <string>

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

        memo::Server aServer(argv[1], argv[2], argv[3]);
        std::cout << "Launching server..." << std::endl;
  	    aServer.run();
  	}
  	catch (std::exception& e)
  	{
  	    std::cerr << "exception: " << e.what() << "\n";
  	}
    std::cout << "Server stopped." << std::endl;

    return 0;
}
