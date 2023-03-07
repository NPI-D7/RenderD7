// rd7cc
#include <fstream>
#include <iostream>
int main(int argc, char *argv[]) {
  std::ofstream result("result.hpp");

  result << "//Result" << std::endl;

  result.close();
}
