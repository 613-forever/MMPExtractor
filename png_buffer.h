#pragma once
#ifndef MMP_EXTRACTOR_PNG_BUFFER_H
#define MMP_EXTRACTOR_PNG_BUFFER_H

#include <memory>
#include <vector>
#include <common613/memory.h>
#include <common613/file_utils.h>

class PngBufferImpl;
class PngBuffer;
using PngBufferPtr = std::unique_ptr<PngBuffer>;

class PngBuffer {
  explicit PngBuffer(PngBufferImpl* pImpl) : impl(pImpl) {}
public:
  static PngBufferPtr create(const common613::File&, const common613::File&);
  static PngBufferPtr create(const common613::Memory& pngMemory, const std::uint32_t (&pos)[4]);

  PngBuffer() = delete;
  PngBuffer(PngBuffer&) = delete;
  ~PngBuffer();
  void fill(const common613::File&, const common613::File&);
  void fill(const common613::Memory& eyesMemory, const common613::Memory& mouthMemory);
  void write(const common613::File&);
  void write(common613::Memory& out);
private:
  PngBufferImpl* impl;
};

#endif //MMP_EXTRACTOR_PNG_BUFFER_H
