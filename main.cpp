#include "HalideRuntime.h"
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include "select.h"
#include "select_cpu.h"

halide_buffer_t *createBufInterleaved(uint8_t* data, int rows, int cols) {
  halide_buffer_t *ret = (halide_buffer_t*)malloc(sizeof(halide_buffer_t));
  halide_dimension_t *dims = (halide_dimension_t*)malloc(3*sizeof(halide_dimension_t));

  ret->host = data;
  ret->set_host_dirty(true);
  ret->type = halide_type_of<uint8_t>();

  ret->dim = dims;
  ret->dimensions = 3;

  ret->dim[0].min = 0;
  ret->dim[0].extent = cols;
  ret->dim[0].stride = 4;

  ret->dim[1].min = 0;
  ret->dim[1].extent = rows;
  ret->dim[1].stride = cols * 4;

  ret->dim[2].min = 0;
  ret->dim[2].extent = 3;
  ret->dim[2].stride = 1;

  return ret;
}

halide_buffer_t *createBuf(uint8_t* data, int rows, int cols) {
  halide_buffer_t *ret = (halide_buffer_t*)malloc(sizeof(halide_buffer_t));
  halide_dimension_t *dims = (halide_dimension_t*)malloc(2*sizeof(halide_dimension_t));

  ret->host = data;
  ret->set_host_dirty(true);
  ret->type = halide_type_of<uint8_t>();

  ret->dim = dims;
  ret->dimensions = 2;

  ret->dim[0].min = 0;
  ret->dim[0].extent = cols;
  ret->dim[0].stride = 1;

  ret->dim[1].min = 0;
  ret->dim[1].extent = rows;
  ret->dim[1].stride = cols;

  return ret;
}

void dumpInterleaved(uint8_t* data, int rows, int cols) {
  for (int y=0; y < rows; y++) {
    for (int x=0; x < rows; x++) {
      for (int c=0; c<4; c++) {
        std::cout << std::setw(4) << (int)(data[4 * (y*cols + x) + c]);
      }
      std::cout << "|";
    }
    std::cout << std::endl;
  }
}

void initInterleaved(uint8_t* data, int rows, int cols, int offset=0) {
  for (int y=0; y < rows; y++) {
    for (int x=0; x < rows; x++) {
      for (int c=0; c<4; c++) {
        data[4 * (y*cols + x) + c] = (uint8_t)(100*x + y + c + offset % 256);
      }
    }
  }
}

void init(uint8_t* data, int rows, int cols) {
  for (int y=0; y < rows; y++) {
    for (int x=0; x < rows; x++) {
      data[(y*cols + x)] = (uint8_t)(x + y % 256);
    }
  }
}

int main(void) {

  int sz = 16;
  uint8_t* src = (uint8_t*)malloc(sizeof(uint8_t) * sz * sz * 4);
  uint8_t* dst = (uint8_t*)malloc(sizeof(uint8_t) * sz * sz * 4);
  uint8_t* out = (uint8_t*)malloc(sizeof(uint8_t) * sz * sz * 4);
  uint8_t* out_cpu = (uint8_t*)malloc(sizeof(uint8_t) * sz * sz * 4);
  uint8_t* msk = (uint8_t*)malloc(sizeof(uint8_t) * sz * sz);

  initInterleaved(src, sz, sz);
  initInterleaved(dst, sz, sz, 100);
  //initInterleaved(out, sz, sz);
  //initInterleaved(out_cpu, sz, sz);
  init(msk, sz, sz);

  auto srcBuf = createBufInterleaved(src, sz, sz);
  auto dstBuf = createBufInterleaved(dst, sz, sz);
  auto outBuf = createBufInterleaved(out, sz, sz);
  auto outCpuBuf = createBufInterleaved(out_cpu, sz, sz);
  auto mskBuf = createBuf(msk, sz, sz);

  select(srcBuf, dstBuf, mskBuf, outBuf);
  select_cpu(srcBuf, dstBuf, mskBuf, outCpuBuf);

  halide_copy_to_host(nullptr, outBuf);

  std::cout <<"dst:\n";
  dumpInterleaved(dst, sz, sz);
  std::cout <<"\nmetal out:\n";
  dumpInterleaved(out, sz, sz);
  std::cout <<"\ncpu out:\n";
  dumpInterleaved(out_cpu, sz, sz);



  return 0;
}
