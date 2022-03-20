#include "png_buffer.h"

#include <opencv2/opencv.hpp>

using namespace std;
using namespace common613;

class PngBufferImpl {
public:
  PngBufferImpl() = default;
  PngBufferImpl(PngBufferImpl&) = delete;
  ~PngBufferImpl() = default;

  void loadFromMemory(const Memory& pngMemory, const uint32_t (&posMemory)[4]);
  void fill(const Memory& eyesMemory, const Memory& mouthMemory);
  void write(Memory& out) const;
private:
  cv::Mat buffer;
  int pos[4]{};
};

void PngBufferImpl::loadFromMemory(const Memory& pngMemory, const uint32_t (&posMemory)[4]) {
  buffer = cv::imdecode(pngMemory, cv::IMREAD_UNCHANGED);
  assert(buffer.type() == CV_8UC4);
//  cv::imshow("body", buffer);
  memcpy(pos, posMemory, 16);
}

void PngBufferImpl::fill(const Memory& eyesMemory, const Memory& mouthMemory) {
  cv::Mat eyes = cv::imdecode(eyesMemory, cv::IMREAD_UNCHANGED);
  cv::Mat mouth = cv::imdecode(mouthMemory, cv::IMREAD_UNCHANGED);
  assert(eyes.type() == CV_8UC4 && mouth.type() == CV_8UC4);
//  cv::imshow("eyes", eyes);
//  cv::imshow("mouth", mouth);
  eyes.copyTo(buffer(cv::Rect(pos[0], pos[1], eyes.cols, eyes.rows)));
  mouth.copyTo(buffer(cv::Rect(pos[2], pos[3], mouth.cols, mouth.rows)));
//  cv::imshow("assembled", buffer);
//  cv::waitKey();
}

void PngBufferImpl::write(Memory& out) const {
  std::vector<int> params{cv::IMWRITE_PNG_COMPRESSION, 9};
  cv::imencode(".png", buffer, out, params);
}

// ---------------------------------------------------------------------

PngBufferPtr PngBuffer::create(const File& body, const File& pos) {
  auto body_data = file::readAll(body);

  uint32_t pos_data[4];
  read(pos, &pos_data);

  return create(body_data, pos_data);
}

PngBufferPtr PngBuffer::create(const Memory& pngMemory, const uint32_t (&pos)[4]) {
  auto* impl = new PngBufferImpl;
  PngBufferPtr result(new PngBuffer(impl));
  impl->loadFromMemory(pngMemory, pos);
  return result;
}

PngBuffer::~PngBuffer() {
  delete impl;
}

void PngBuffer::fill(const File& eyes, const File& mouth) {
  auto eyes_data = file::readAll(eyes), mouth_data = file::readAll(mouth);
  fill(eyes_data, mouth_data);
}

void PngBuffer::fill(const Memory& eyesMemory, const Memory& mouthMemory) {
  impl->fill(eyesMemory, mouthMemory);
}

void PngBuffer::write(const File& file) const {
  Memory buffer;
  write(buffer);
  file::write(file, buffer.data(), buffer.size());
}

void PngBuffer::write(Memory& out) const {
  impl->write(out);
}
