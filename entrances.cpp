#include "entrances.h"

#include <vector>
#include <set>
#include <cstring>
#include <common613/file_utils.h>
#include <iostream>
#include "png_buffer.h"

using namespace common613::file;
using common613::filesystem::path;
using std::string;
using std::vector;

void assemble_single(const path& base_dir,
                     const string& body_name,
                     int index,
                     int eyes,
                     int mouth,
                     const path& output_file) {
  PngBufferPtr buffer;
  {
    File body_file = open(base_dir / (body_name + ".png"), "rb");
    File pos_file = open(base_dir / (body_name + ".POS"), "rb");
    buffer = PngBuffer::create(body_file, pos_file);
  }

  {
    File eyes_file = open(base_dir / fmt::format("{}_{:02d}_E{:1d}.png", body_name, index, eyes), "rb");
    File mouth_file = open(base_dir / fmt::format( "{}_{:02d}_M{:1d}.png", body_name, index, mouth), "rb");

    buffer->fill(eyes_file, mouth_file);
  }

  {
    File target = open(output_file, "wb");
    buffer->write(target);
  }
}

vector<string> parse_gim_data(const File& file); // "GIMDATA.DAT"
string parse_pos_data(const File& file); // "POSDATA.DAT"

string get_base_name(const string& str) {
  return str.substr(0, str.size() - 4); // "talkchara??_??.gim"
}

void collect_indices(const vector<string>& gim_data, vector<int>& indices) {
  std::set<int> indices_set;
  std::transform(gim_data.begin() + 1, gim_data.end(),
                 std::inserter(indices_set, indices_set.end()),
                 [](const std::string& filename) {
                   return std::stoi(filename.substr(15, 2));
                 });
  indices.clear();
  indices.reserve(gim_data.size());
  std::copy(indices_set.begin(), indices_set.end(), std::back_inserter(indices));
}

void assemble_scan(const path& base_dir, const path& output_dir) {
  PngBufferPtr buffer;
  vector<int> indices;
  string base_name;
  {
    vector<string> gim_data = parse_gim_data(open(base_dir / "GIMDATA.DAT", "rb"));
    base_name = get_base_name(gim_data[0]);
    File body_file = open(base_dir / (base_name + ".png"), "rb");
    collect_indices(gim_data, indices);
    File pos_file = open(base_dir / parse_pos_data(open(base_dir / "POSDATA.DAT", "rb")), "rb");
    buffer = PngBuffer::create(body_file, pos_file);
  }

  for (int index : indices) {
    try {
      File eyes_file = open(base_dir / fmt::format("{}_{:02d}_E0.png", base_name, index), "rb");
      File mouth_file = open(base_dir / fmt::format("{}_{:02d}_M0.png", base_name, index), "rb");

      buffer->fill(eyes_file, mouth_file);

      string target_name = fmt::format("{}_{:02d}.png", base_name, index);
      File target = open(output_dir / target_name, "wb");
      buffer->write(target);
      std::clog << "assemble : " << target_name << std::endl;
    } catch (std::runtime_error& e) {
      std::clog << "A file skipped due to error: " << e.what() << std::endl;
    }
  }
}

vector<string> parse_gim_data(const File& file) {
  std::uint16_t name_length;
  vector<char> buffer(0x20); // resize later if not enough
  vector<string> results;
  for (;;) {
    read(file, &name_length, 1);
    if (name_length >= buffer.size()) {
      buffer.resize(name_length + 1);
    }
    std::size_t read_length = read(file, buffer.data(), std::nothrow, name_length);
    buffer[read_length] = '\0';
    if (read_length < name_length) {
      std::clog << "Incomplete file name: \"" << buffer.data() << "\" with length " << read_length
                << " (expected: " << name_length << ")" << std::endl;
    }
    if (std::memcmp("END", buffer.data(), 4) == 0) break;
    string capital;
    std::transform(buffer.begin(), buffer.end(), buffer.begin(), [](char c) { return std::toupper(c); });
    results.emplace_back(buffer.data(), read_length);
  }
  return results;
}

string parse_pos_data(const File& file) {
  std::uint16_t name_length;
  read(file, &name_length);
  string result;
  result.resize(name_length);
  std::size_t read_length = read(file, result.data(), std::nothrow, name_length);
  if (read_length < name_length) {
    std::clog << "Incomplete file name: \"" << result << "\" with length " << read_length
              << " (expected: " << name_length << ")" << std::endl;
  }
  std::transform(result.begin(), result.end(), result.begin(), [](char c) { return std::toupper(c); });
  return result;
}
