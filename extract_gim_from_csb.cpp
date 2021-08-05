#include <iostream>
#include <cstring>
#include <common613/file_utils.h>

using namespace std;
using common613::filesystem::path;
using common613::filesystem::exists;
using common613::filesystem::create_directories;
using namespace common613::file;

int main(int argc, char* argv[]) {
  if (argc < 2) return 2;

  File file = open(argv[1], "rb");
  char buffer[4];
  path dir(argv[2]);
  if (!exists(dir)) {
    std::clog << "warning: target dir does not exists. check if it is erroneous." << std::endl;
  }
  while (!eof(file)) {
    size_t count = read(file, buffer, std::nothrow, 4); // 4 = alignment.
    if (count < 4) continue;
    if (strncmp(buffer, "MIG.", 4) == 0) {
      std::clog << "\"MIG.\" detected." << std::endl;
      seek(file, -0x4, SEEK_CUR);
      char magic[12];
      if (read(file, &magic, std::nothrow) == 1
          && memcmp(magic, "MIG.00.1PSP\0", 12) == 0) {
        seek(file, 0x8, SEEK_CUR);
        uint32_t filesize;
        read(file, &filesize);
        filesize += 16;

        seek(file, -0x30, SEEK_CUR);
        char filename[21] = {'\0'};
        read(file, filename, 16);
        strcpy(filename + strlen(filename), ".gim");

        seek(file, 0x8, SEEK_CUR);
        vector<char> gim(filesize);
        read(file, gim.data(), filesize);

        File outFile = open(dir / filename, "wb");
        write(outFile, gim.data(), filesize);
        outFile.get();
      }
    }
  }
  return 0;
}
