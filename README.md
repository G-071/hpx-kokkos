# HPX/Kokkos interoperability library

WARNING: This repo is work in progress and should not be relied on for
anything. Please read the [known limitations](#known-limitations).

## What?

A header-only library for HPX/Kokkos interoperability. It provides:

- `async` versions of `Kokkos::parallel_for`, `parallel_reduce`, and `parallel_scan`
- `async` version of `Kokkos::deep_copy`
- Kokkos executors that forward work to corresponding Kokkos execution spaces

## How?

This is a header-only library and does not need compiling. However, it is
recommended that it be installed using traditional CMake commands:

```
# In repository root
mkdir -p build
cd build
cmake .. \
    -DCMAKE_BUILD_TYPE=<build_type> \
    -DCMAKE_CXX_COMPILER=<compiler> \
    -DHPX_DIR=<hpx_dir> \
    -DKokkos_DIR=<kokkos_dir>
```

where `<build_type>` should be the same build type used to build HPX and
Kokkos, `<compiler>` should be the same compiler used to build HPX and Kokkos,
`<hpx_dir>` and `<kokkos_dir>` should point to the directories containing HPX
and Kokkos configuration files, respectively.

In your CMake configuration, add `find_package(HPXKokkos REQUIRED)` and link
your targets to `HPXKokkos::hpx_kokkos`. Finally, include `hpx/kokkos.hpp` in
your code.

Tests can be enabled with the CMake option `HPX_KOKKOS_ENABLE_TESTS`.

# Requirements

- CMake version 3.13 or newer
- HPX version 1.5.0 or newer
- Kokkos version 3.2.0 or newer
  - The build should have `Kokkos_ENABLE_HPX_ASYNC_DISPATCH=ON`

For CUDA support HPX and Kokkos should be built with CUDA support. See their
respective documentation for enabling CUDA support. CUDA support requires
`Kokkos_ENABLE_CUDA_LAMBDA=ON`. The library can be used with other Kokkos
execution spaces, but only the HPX and CUDA backends are currently
asynchronous. HIP support is planned.

# API

The only supported header is `hpx/kokkos.hpp`. All other headers may change
without notice.

The following functions follow the same API as the corresponding Kokkos
functions. All execution spaces except HPX and CUDA are currently blocking and
only return ready futures.

```
namespace hpx { namespace kokkos {
hpx::shared_future<void> parallel_for_async(...);
hpx::shared_future<void> parallel_reduce_async(...);
hpx::shared_future<void> parallel_scan_async(...);
hpx::shared_future<void> deep_copy_async(...);
}}
```

The following executors correspond to Kokkos execution spaces. The executor is
only defined if the corresponding execution space is enabled in Kokkos.

```
namespace hpx { namespace kokkos {
// The following are always defined
class default_executor;
class default_host_executor;

// The following are conditionally defined
class cuda_executor;
class hip_executor;
class hpx_executor;
class openmp_executor;
class rocm_executor;
class serial_executor;
}}
```

The following is a helper function for creating execution space instances that
are independent. It is allowed to return the same execution space instance on
subsequent invocations. For CUDA it returns execution space instances that have
different streams. For HPX it returns execution space instances with different
internal futures.

```
namespace hpx { namespace kokkos {
template <typename ExecutionSpace = Kokkos::DefaultExecutionSpace>
ExecutionSpace make_execution_space();
}}
```

## Known limitations

The following are known limitations of the library. If one of them is
particularly important for your use case, please open an issue and we'll
prioritize getting it fixed for you.

- Compilation with `nvcc` is likely not to work. Prefer `clang` for compiling
  CUDA code.
- Only the HPX and CUDA execution spaces are asynchronous. Parallel algorithms
  with other execution spaces always block and return a ready future (where
  appropriate).
- Not all HPX parallel algorithms can be used with the Kokkos executors. Most
  embarassingly data-parallel algorithms are likely to work. Currently tested
  algorithms are:
  - `hpx::for_each`
  - `hpx::for_loop`
  - `hpx::transform`
- The Kokkos executors do not support continuations (`then_execute` and
  `bulk_then_execute`). One can instead call the required function from a host
  continuation.
- `Kokkos::View` construction and destruction (when reference count goes to
  zero) are generally blocking operations and this library does not currently
  try to solve this problem. Workarounds are: create all required views upfront
  or use unmanaged views and handle allocation and deallocation manually.
- `bulk_async_execute` uses a `Kokkos::View` internally which leads to blocking
  the full execution space when the bulk task is finished. The HPX or Kokkos
  parallel algorithms do not have this problem inherently, but care must still be
  taken with user-defined views to avoid blocking (see previous limitation).
