#include "src/volcano.hpp"
#include <cstdlib>
#include <iostream>

int main() {
  Volcano volcano;

  try {
    volcano.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
