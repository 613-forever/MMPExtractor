#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <common613/file_utils.h>
#include <common613/struct_size_check.h>

using namespace std;
using namespace common613::file;

struct GIMGeneralBlockHeader {
  uint16_t block_id, block_unk;
  uint32_t block_size, block_header_next, block_data_offset;
  COMMON613_INJECT_SIZE_FIELD(0x10);
};
COMMON613_CHECK_SIZE(GIMGeneralBlockHeader);

struct GIMImageHeader {
  uint16_t header, b45_unk1, image_format, pixel_order, width, height,
      bpp_align, pitch_align, height_align, b45_unk2;
  uint32_t b45_unk3, index_start, pixels_start, pixels_end, plane_mask;
  uint16_t level_type, level_count, frame_type, frame_count;
  int8_t data_head[16];
  COMMON613_INJECT_SIZE_FIELD(0x40);
};
COMMON613_CHECK_SIZE(GIMImageHeader);

struct ABGR4444{
  uint8_t data[2];
  [[nodiscard]] uint8_t r() const { return ((data[0]) & 0xF) * 0x11; }
  [[nodiscard]] uint8_t g() const { return ((data[0] >> 4) & 0xF) * 0x11; }
  [[nodiscard]] uint8_t b() const { return ((data[1]) & 0xF) * 0x11; }
  [[nodiscard]] uint8_t a() const { return ((data[1] >> 4) & 0x0F) * 0x11; }
  COMMON613_INJECT_SIZE_FIELD(2);
};
COMMON613_CHECK_SIZE(ABGR4444);
ostream& operator<<(ostream& os, const ABGR4444& data) {
  os << "[" << int(data.r()) << "," << int(data.g()) << "," << int(data.b()) << ";" << int(data.a()) << "] 0x "
      << hex
      << setfill('0') << setw(2) << int(data.data[0]) << ' '
      << setfill('0') << setw(2) << int(data.data[1]) << ' '
      << dec;
  return os;
}

struct ABGR8888{
  uint8_t data[4];
  [[nodiscard]] uint8_t r() const { return ((data[0])); }
  [[nodiscard]] uint8_t g() const { return ((data[1])); }
  [[nodiscard]] uint8_t b() const { return ((data[2])); }
  [[nodiscard]] uint8_t a() const { return ((data[3])); }
  COMMON613_INJECT_SIZE_FIELD(4);
};
COMMON613_CHECK_SIZE(ABGR8888);
ostream& operator<<(ostream& os, const ABGR8888& data) {
  os << "[" << int(data.r()) << "," << int(data.g()) << "," << int(data.b()) << ";" << int(data.a()) << "] 0x "
      << hex
      << setfill('0') << setw(2) << int(data.data[0]) << ' '
      << setfill('0') << setw(2) << int(data.data[1]) << ' '
      << setfill('0') << setw(2) << int(data.data[2]) << ' '
      << setfill('0') << setw(2) << int(data.data[3]) << ' '
      << dec;
  return os;
}

int main(int argc, char* argv[]) {
  if (argc < 2) return 2;

  File file = open(argv[1], "rb");

  long image_block_start, palette_block_start;
  GIMGeneralBlockHeader image_block_header{}, palette_block_header{};
  GIMImageHeader image_header{}, palette_header{};

  image_block_start = 0x30;
  seek(file, image_block_start, SEEK_SET);
  read(file, &image_block_header);
  read(file, &image_header);

  palette_block_start = image_block_start + static_cast<long>(image_block_header.block_size);
  seek(file, palette_block_start, SEEK_SET);
  read(file, &palette_block_header);
  read(file, &palette_header);

  long color_count = palette_header.height * palette_header.width;
  vector<cv::Vec4b> palette;
  palette.reserve(color_count);

  // in fact, only ABGR4444 - index16 will appear.
  COMMON613_REQUIRE(palette_header.image_format == 2 || palette_header.image_format == 3,
                    "required palette pixel format ABGR4444 (2) or ABGR8888 (3), but found {}", palette_header.image_format);
  COMMON613_REQUIRE(image_header.image_format == 5 || image_header.image_format == 6,
                    "required image pixel format index8 (5) or index16 (6), but found {}", image_header.image_format);
  COMMON613_REQUIRE(image_header.pixel_order == 0,
                    "required image pixel order normal (0), but found {}", image_header.pixel_order);

  long palette_pixels_start = palette_block_start +
      static_cast<long>(GIMGeneralBlockHeader::size + palette_header.pixels_start);
  seek(file, palette_pixels_start, SEEK_SET);

  cout << "palette format: " << palette_header.image_format << endl;
  switch (palette_header.image_format) {
  case 2: {
    vector<ABGR4444> raw_color(color_count);
    read(file, raw_color.data(), color_count);
    transform(raw_color.begin(), raw_color.end(), back_inserter(palette), [](const ABGR4444& c) {
      return cv::Vec4b(c.b(), c.g(), c.r(), c.a());
    });
  } break;
  case 3: {
    vector<ABGR8888> raw_color(color_count);
    read(file, raw_color.data(), color_count);
    transform(raw_color.begin(), raw_color.end(), back_inserter(palette), [](const ABGR8888& c) {
      return cv::Vec4b(c.b(), c.g(), c.r(), c.a());
    });
  } break;
  default:
    abort();
  }

  long image_pixels_start = image_block_start +
      static_cast<long>(GIMGeneralBlockHeader::size + image_header.pixels_start);
  cv::Mat mat(image_header.height, image_header.width, CV_8UC4);
  seek(file, image_pixels_start, SEEK_SET);

  cout << "image format: " << image_header.image_format << endl;
  switch (image_header.image_format) {
  case 5: {
    uint8_t index;
    for (int i = 0; i < image_header.width / 16; ++i) {
      for (int j = 0; j < image_header.height / 8; ++j) {
        for (int k = 0; k < 16; ++k) {
          for (int l = 0; l < 8; ++l) {
            read(file, &index);
            mat.at<cv::Vec4b>(j * 8 + l, i * 16 + k) = palette[index];
          }
        }
      }
    }
  } break;
  case 6: {
    uint16_t index;
    for (int i = 0; i < image_header.height; ++i) {
      for (int j = 0; j < image_header.width; ++j) {
        read(file, &index);
        mat.at<cv::Vec4b>(i, j) = palette[index];
      }
    }
  } break;
  default:
    COMMON613_FATAL("Unexpected branch: {}", image_header.image_format);
  }

  vector<unsigned char> buf;
  vector<int> params{cv::IMWRITE_PNG_COMPRESSION, 9};
  cv::imencode(".png", mat, buf, params);
  File outFile = open(argv[2], "wb");
  write(outFile, buf.data(), buf.size());

  return 0;
}
