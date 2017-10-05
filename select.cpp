#include "Halide.h"

#define NEWER_HALIDE

using namespace Halide;

class SelectErr : public Halide::Generator<SelectErr> {

public:
        Input<Buffer<uint8_t>> src{"src", 3};
        Input<Buffer<uint8_t>> dst{"dst", 3};
        Input<Buffer<uint8_t>> mask{"mask", 2};

        Var x{"x"}, y{"y"}, c{"c"};

        Output<Buffer<uint8_t>> output{"output", 3};

  void generate() {
    Expr d = cast<int16_t>(src(x, y, c) - 128);
    Expr a = cast<uint8_t>(d + (d/64));
    Expr b = select(dst(x, y, c) < a, mask(x, y), dst(x, y, c));
    output(x, y, c) = select(src(x, y, c) > 128, b, dst(x, y, c));
  }
  void schedule() {

    src.dim(0).set_stride(4).dim(2).set_stride(1);
    dst.dim(0).set_stride(4).dim(2).set_stride(1);
    src.dim(2).set_bounds(0, 3);
    dst.dim(2).set_bounds(0, 3);

    output.dim(0).set_stride(4).dim(2).set_stride(1);
    output.dim(2).set_bounds(0, 3);

    output.reorder(c, x, y).unroll(c);

    Var tx, ty;
    if (this->get_target().has_gpu_feature())
      output.gpu_tile(x, y, tx, ty, 8, 8);
  }

};

#ifdef NEWER_HALIDE
HALIDE_REGISTER_GENERATOR(SelectErr, select_err);
#else
RegisterGenerator<SelectErr> select_err{"select_err"};
#endif
