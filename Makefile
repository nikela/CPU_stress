CC=gcc
CFLAGS=-O3
INCDIR=/usr/include/mkl
LIBDIR=/usr/lib/x86_64-linux-gnu
LIBS=-lm -lmkl_avx512 -lmkl_rt

%.o: %.c
	$(CC) $(CFLAGS) -I$(INCDIR) -c -o $@ $<
gemm: gemm.o
	$(CC) -o gemm gemm.o -L$(LIBDIR) $(LIBS)

clean: 
	rm gemm.o gemm
