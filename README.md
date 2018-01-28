# clcc - an NVIDIA OpenCL command line compiler

## History

### Leith Bade:

I was having problems with the NVIDIA compiler crashing on my code. As I was poking around trying to figure out what was causing the crash, I figured out the shared library interface to the NVIDIA OpenCL clang/llvm compiler.

I created a little program that uses this compiler (bundled with the drivers) to do command line compiling of code.

I hope this will be useful to people who want to integrate a OpenCL C to PTX compiler into their build chain. You might be able to run ptxas on the resulting PTX file to get cubin, but I have not yet investigated if clCreateProgramWithBinary will accept cubin.

The other useful feature is that you can control the compiler options to select a different target CUDA Compute Level (sm_XX) than the card currently installed. You can even run the compiler without needing a GPU installed at all - just extract needed nvcompiler.dll/nvcompiler32.dll/libnvidia-compiler.so file from the latest driver installer. I imagine this will be useful for automated/dedicated build machines that likely lack a GPU.

Feel free to discuss on the NVIDIA forums:
http://forums.nvidia.com/index.php?showtopic=188884

### David Castells-Rufas

I was interested in compiling and testing OpenCL kernels on different platforms and especially receive feedback about the used resources.

I noticed that NVIDIA OpenCL gives different information wether it is executed from the former clcc implementation and the OpenCL runtime compiler clBuildProgram logs.

So I added a way to select wether Leith direct shared library access method is used or classic OpenCL runtime. 

I also modified the command line interface to be easier to add new flags. 

I also introduced a compilation flow using cmake. I tested it in Cygwin and Ubuntu Linux.

Finally I moved from C to C++, to reuse OpenCL functions that use STL classes.

## Execution

The format of the command is this:

clcc [<options>] [--flags "<build-options>"] -i input.cl -o output.ptx

<options> can be 

--help      shows the usage 

--opencl-runtime    uses the OpenCL runtime to compile the code. Otherwise it uses the original direct access to NVIDIA library.

<build-options> - a quoted string containing the build options to pass to the NVIDIA compiler 

It accepts all the OpenCL 1.0/1.1 clBuildProgram options (http://www.khronos.org/registry/cl/sdk/1.1/docs/man/xhtml/clBuildProgram.html), the NVIDIA extended ones from cl_nv_compiler_options (http://developer.download.nvidia.com/compute/cuda/3_2_prod/toolkit/docs/OpenCL_Extensions/cl_nv_compiler_options.txt), and these hidden extras:
-cl-nv-arch sm_XX - selects the target CUDA Compute Level architecture to compile for (sm_10 for 1.0, sm_11 for 1.1, sm_12 for 1.2, sm_13 for 1.3, sm_20 for 2.0, sm_21 for 2.1, sm_30 for 3.0 and sm_35 for 3.5)
-cl-nv-cstd=CLX.X - selects the target OpenCL C language version (CL1.0 or CL1.1)

See the flags section below, for more info.

input.cl - the source OpenCL C file

output.ptx - the destination PTX file

Currently I only support one input per PTX file, but the API supports multiple inputs so I may add that feature.

Try it out, test it, use it etc. and give me feedback, bugs, suggestions etc via GitHub (or on the NVIDIA forum thread).

### IMPORTANT NOTE

NVIDIA compiler chaches the results of previous compilations, so that you can not receive any feedback
from -cl-nv-verbose if the compiler uses the cached version. To force the recompilation you must flush the cache.

In my Linux system this cache is in /home/<user>/.nv/ComputeCache


## building

### Visual Studio 2008 for Windows 

Open the clcc.sln solutions

### Cygwin

Run the cmake_make.sh script, it will create a build directory with a Makefile that can be used to compile

### Netbeans 8.2

A prebuild step invoking cmake_make.sh should be specified. Alternative this step can be run from a Cygwin console.

Set the directory used during compilation to "build" and use standard make, and make clean.

### Linux

Run the cmake_make.sh script, it will create a build directory with a Makefile that can be used to compile


## Flags

There is little information about the flags accepted by the OpenCL compilers. Here I try to review some


### General Flags

| Flag | Description | Comments |
|------|-------------|----------|
| -cl-opt-disable | 
| -cl-single-precision-constant
| -cl-denorms-are-zero
| -cl-fp32-correctly-rounded-divide-sqrt
| -cl-mad-enable
| -cl-no-signed-zeros
| -cl-unsafe-math-optimizations
| -cl-finite-math-only
| -cl-fast-relaxed-math

### NVIDIA flags

| Flag | Description | Comments |
|------|-------------|----------|
| -cl-nv-arch | architecture | |
| -cl-nv-cstd=<version> | version is the OpenCL version | can be CL1.0, CL1.1, etc. |
| -cl-nv-maxrregcount <N> | Specify the maximum number of registers that GPU functions can use. | It fails in some systems|
| -cl-nv-opt-level <N> | 0 (no optimization). | |
| -cl-nv-verbose | | Does not show much information |