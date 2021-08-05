#include <common613/file_utils.h>

using namespace std;
using namespace common613::file;

int main(int argc, char* argv[]) {
  if (argc < 2) return 2;

  std::uint32_t pos[4];
  File file = open(argv[1], "rb");
  fread(pos, 4, 4, file.get());

  File outFile = open(argv[2], "w");
  fprintf(outFile.get(), "%d\n%d\n%d\n%d\n", pos[0], pos[1], pos[2], pos[3]);

  return 0;
}
