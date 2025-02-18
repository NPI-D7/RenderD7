#include <filesystem>
#include <fstream>
#include <map>
#include <renderd7/nimg.hpp>
#include <sstream>

// Use an Npi simplifier cause I am lazy
#define reca_cc(x) reinterpret_cast<const char*>(x)
#define reca_c(x) reinterpret_cast<char*>(x)
#define pak32(q, w, e, r)                                             \
  ((((q) & 0xff) << 0) | (((w) & 0xff) << 8) | (((e) & 0xff) << 16) | \
   (((r) & 0xff) << 24))

// Stupid RLE Algorithm
void npi_compress(std::vector<unsigned char>& ret,
                  const std::vector<unsigned char>& in) {
  unsigned char counter = 1;
  unsigned char tmp = in[0];
  for (size_t i = 1; i < in.size(); ++i) {
    if (tmp == in[i]) {
      counter++;
    } else {
      ret.push_back(counter);
      ret.push_back(tmp);
      counter = 1;
      tmp = in[i];
    }
    if (counter == 255) {
      ret.push_back(counter);
      ret.push_back(tmp);
      counter = 0;
    }
  }
  if (counter > 0) {
    ret.push_back(counter);
    ret.push_back(tmp);
  }
}

void npi_decompress(std::vector<unsigned char>& ret,
                    const std::vector<unsigned char>& in) {
  // Size is sus
  if ((in.size() % 2) != 0) return;
  for (size_t i = 0; i < in.size(); i += 2) {
    int count = in[i];
    int value = in[i + 1];
    for (int c = 0; c < count; c++) {
      ret.push_back(value);
    }
  }
}

namespace RenderD7 {
nimg NIMG_Load(std::string path) {
  nimg res;
  std::ifstream fin(path, std::ios::in | std::ios::binary);
  // Check magic
  fin.read(reca_c(&res.magic), sizeof(uint32_t));
  if (res.magic != NPI_NIMG_) {
    std::cout << path << " is invalid!" << std::endl;
    return res;
  }
  // Read Information
  fin.read(reca_c(&res.width), sizeof(int));
  fin.read(reca_c(&res.height), sizeof(int));
  fin.read(reca_c(&res.format), sizeof(int));
  fin.read(reca_c(&res.compression), sizeof(int));
  // Read Pixeldata
  if (res.compression == 1) {
    std::vector<unsigned char> tb;
    int pb_size = 0;
    fin.read(reca_c(&pb_size), sizeof(int));
    tb.resize(pb_size);
    fin.read(reca_c(tb.data()), pb_size);
    npi_decompress(res.pixel_buffer, tb);
  } else {
    int pb_size = 0;
    fin.read(reca_c(&pb_size), sizeof(int));
    res.pixel_buffer.resize(pb_size);
    fin.read(reca_c(res.pixel_buffer.data()), pb_size);
  }

  // Close stream
  fin.close();
  // Return the loadet imaeg
  return res;
}

nimg NIMG_LoadFromMem(unsigned char* buffer, size_t bf_size) { return nimg(); }

void NIMG_Save(nimg image, std::string path) {
  std::ofstream fout(path);
  // Write Magic
  fout.write(reca_cc(&image.magic), sizeof(uint32_t));
  // Write Information
  fout.write(reca_cc(&image.width), sizeof(int));
  fout.write(reca_cc(&image.height), sizeof(int));
  fout.write(reca_cc(&image.format), sizeof(int));
  fout.write(reca_cc(&image.compression), sizeof(int));
  std::vector<unsigned char> test;
  // Write pixels
  if (image.compression == 1) {
    std::vector<unsigned char> tb;
    npi_compress(tb, image.pixel_buffer);
    int buf_szs = tb.size();
    fout.write(reca_cc(&buf_szs), sizeof(int));  // buf_size
    fout.write(reca_cc(tb.data()),
               tb.size());  // buffer
  } else {
    int buf_szs = image.pixel_buffer.size();
    fout.write(reca_cc(&buf_szs), sizeof(int));  // buf_size
    fout.write(reca_cc(image.pixel_buffer.data()),
               image.pixel_buffer.size());  // buffer
  }

  // Close stream
  fout.close();
}
}  // namespace RenderD7