#include <iostream>
#include "AsioConfig.h"
int main(int argc, char** argv) {
    if (argc < 2) {
    // report version
    std::cout << argv[0] << " Version " << Asio_VERSION_MAJOR << "."
              << Asio_VERSION_MINOR << std::endl;
    std::cout << "Usage: " << argv[0] << " number" << std::endl;
  }
    std::cout << "Hello, world!\n";
}