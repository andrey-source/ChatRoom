#include "application.h"



int main(int argc, char* argv[])
{
    // client cl("../voice_data/", 0, 1,1024, 0, 44100);


    // try  
    // {
    //   if (argc == 1) {
    //     client cl;
    //     cl.run();
    //     return 0;
    //   }
    //   if (argc == 2) {
    //     client cl(argv[1]);
    //     cl.run();
    //     return 0;
    //   }
    // }
    // catch (std::exception &e) {
    //   std::cerr << "exception: "<< e.what() << "\n";
    // }
    // return 0;

    client cl;
    cl.run();


  return 0;
}

    // try
    // {
    //     // Check command line arguments.
    //     if (argc != 4)
    //     {
    //         std::cerr << "Usage: http_server <address> <port> <threads>\n";
    //         std::cerr << "  For IPv4, try:\n";
    //         std::cerr << "    receiver 0.0.0.0 80 1 \n";
    //         std::cerr << "  For IPv6, try:\n";
    //         std::cerr << "    receiver 0::0 80 1 \n";
    //         return 1;
    //     }

    //     // Initialise the server.
    //     std::size_t num_threads = boost::lexical_cast<std::size_t>(argv[3]);
    //     auto s = std::make_shared<server3::server>(argv[1], argv[2], num_threads);

    //     // Run the server until stopped.
    //     s->run();
    // }
    // catch (std::exception& e)
    // {
    //     std::cerr << "exception: " << e.what() << "\n";
    // }