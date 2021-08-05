#include <iostream>
#include <cstring>
#include "entrances.h"

using std::string;
using namespace common613;

void print_help() {
  std::cout
      << "For single:\tMMPExtractor_StandAssemble --single <name> <index> <eyes> <mouth> [<base_dir>] [<output>]\n"
      << "Uses a <name> to specify the body file, pos file and name prefixes for segments.\n"
      << "Uses an <index> to specify the expression.\n"
      << "Both <eyes> and <mouth> should be an suffix 0,1,2,3, to specify frames.\n"
      << "For dir:   \tMMPExtractor_StandAssemble --scan [<base_dir>] [<output_dir>]\n"
      << "Scans <base_dir> using the POSDATA.DAT and GIMDATA.DAT beneath it. Use current as default.\n"
      << std::endl;
}

int main(int argc, char* argv[]) {
  if (std::strcmp(argv[1], "--help") == 0 || std::strcmp(argv[1], "-h") == 0) {
    print_help();
    return 0;
  }

  if (std::strcmp(argv[1], "--single") == 0) {
    if (argc < 7) {
      print_help();
      return 2;
    }
    char* end;
    int index, eyes, mouth;
    index = std::strtol(argv[4], &end, 10);
    eyes = std::strtol(argv[5], &end, 10);
    mouth = std::strtol(argv[6], &end, 10);
    if (eyes < 0 || eyes > 3 || mouth < 0 || mouth > 2) {
      print_help();
      return 2;
    }
    filesystem::path base_dir(argc > 7 ? argv[7] : filesystem::current_path());
    if (!filesystem::exists(base_dir)) {
      std::clog << "<base_dir> does not exist!" << std::endl;
      return 1;
    }
    filesystem::path output_path(argc > 8 ? argv[8] : filesystem::current_path() / "output.png");
    try {
      assemble_single(base_dir, argv[1], index, eyes, mouth, output_path);
    } catch (std::runtime_error& e) {
      std::clog << "Runtime error: " << e.what() << std::endl;
      return 1;
    }
  } else {
    filesystem::path base_dir(argc > 2 ? argv[2] : filesystem::current_path());
    if (!filesystem::exists(base_dir)) {
      std::clog << "<base_dir> does not exist!" << std::endl;
      return 1;
    }
    filesystem::path output_dir(argc > 3 ? argv[3] : filesystem::current_path());
    if (!filesystem::exists(output_dir)) {
      filesystem::create_directories(output_dir);
    }
    try {
      assemble_scan(base_dir, output_dir);
    } catch (std::runtime_error& e) {
      std::clog << "Runtime error: " << e.what() << std::endl;
      return 1;
    }
  }

  return 0;
}
