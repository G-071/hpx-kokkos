///////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2019-2020 ETH Zurich
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///////////////////////////////////////////////////////////////////////////////

/// \file Forwarding header. Includes all Kokkos helper functionality.
/// Applications should include this header only.

#pragma once

#include <hpx/kokkos/config.hpp>
#include <hpx/kokkos/deep_copy.hpp>
#include <hpx/kokkos/detail/version.hpp>
#include <hpx/kokkos/execution_spaces.hpp>
#include <hpx/kokkos/executors.hpp>
#include <hpx/kokkos/future.hpp>
#include <hpx/kokkos/hpx_algorithms.hpp>
#include <hpx/kokkos/import.hpp>
#include <hpx/kokkos/instance_helper.hpp>
#include <hpx/kokkos/kokkos_algorithms.hpp>
#include <hpx/kokkos/policy.hpp>
#include <hpx/kokkos/view.hpp>
