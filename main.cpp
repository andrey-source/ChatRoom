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

    application cl;
    cl.run();


  return 0;
}
