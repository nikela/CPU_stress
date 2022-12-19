CC=gcc
CFLAGS=-O3
INCDIR=/various/common_tools/intel-icc-and-tools/mkl/include
LIBDIR=/various/common_tools/intel-icc-and-tools/mkl/lib/intel64
LIBS=-lm -lmkl_rt

%.o: %.c
    $(CC) $(CFLAGS) -I$(INCDIR) -c -o $@ $<
gemm: gemm.o
    $(CC) -o gemm gemm.o -L$(LIBDIR) $(LIBS)

clean:
	rm gemm.o gemm
