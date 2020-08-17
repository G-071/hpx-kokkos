//  Copyright (c) 2019-2020 ETH Zurich
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file Contains specializations of HPX algorithms for the Kokkos execution
/// policy.

#pragma once

#include <hpx/kokkos/detail/logging.hpp>
#include <hpx/kokkos/policy.hpp>

#include <hpx/algorithm.hpp>

#include <Kokkos_Core.hpp>

#include <utility>

namespace hpx {
namespace kokkos {
namespace detail {

template <typename ExecutionSpace, typename IterB, typename IterE, typename F>
hpx::shared_future<void> for_each_helper(ExecutionSpace &&instance, IterB first,
                                         IterE last, F &&f) {
  return parallel_for_async(
      Kokkos::Experimental::require(
          Kokkos::RangePolicy<ExecutionSpace>(instance, 0,
                                              std::distance(first, last)),
          Kokkos::Experimental::WorkItemProperty::HintLightWeight),
      KOKKOS_LAMBDA(int const i) {
        HPX_KOKKOS_DETAIL_LOG("for_each i = %d", i);
        hpx::invoke(f, *(first + i));
      });
}

template <typename ExecutionSpace, typename Range, typename F,
          typename std::enable_if<Kokkos::is_execution_policy<
                                      typename std::decay<Range>::type>::value,
                                  int>::type = 0>
hpx::shared_future<void> for_each_range_helper(ExecutionSpace &&instance,
                                               Range &&range, F &&f) {
  return parallel_for_async(
      Kokkos::Experimental::require(
          std::forward<Range>(range),
          Kokkos::Experimental::WorkItemProperty::HintLightWeight),
      std::forward<F>(f));
}

template <
    typename ExecutionSpace, typename Range, typename F,
    typename std::enable_if<
        !Kokkos::is_execution_policy<typename std::decay<Range>::type>::value &&
            hpx::traits::is_range<Range>::value,
        int>::type = 0>
hpx::shared_future<void> for_each_range_helper(ExecutionSpace &&instance,
                                               Range &&range, F &&f) {
  return for_each_helper(std::forward<ExecutionSpace>(instance),
                         hpx::util::begin(range), hpx::util::end(range),
                         std::forward<F>(f));
}
} // namespace detail

// For each non-range overloads
template <typename Iter, typename F>
void tag_invoke(hpx::for_each_t, hpx::kokkos::kokkos_policy policy, Iter first,
                Iter last, F &&f) {

  detail::for_each_helper(policy.executor().instance(), first, last,
                          std::forward<F>(f))
      .get();
}

template <typename Iter, typename F>
hpx::shared_future<void> tag_invoke(hpx::for_each_t,
                                    hpx::kokkos::kokkos_task_policy policy,
                                    Iter first, Iter last, F &&f) {
  return detail::for_each_helper(policy.executor().instance(), first, last, f);
}

template <typename Executor, typename Parameters, typename Iter, typename F>
void tag_invoke(hpx::for_each_t,
                hpx::kokkos::kokkos_policy_shim<Executor, Parameters> policy,
                Iter first, Iter last, F &&f) {

  detail::for_each_helper(policy.executor().instance(), first, last,
                          std::forward<F>(f))
      .get();
}

template <typename Executor, typename Parameters, typename Iter, typename F>
hpx::shared_future<void>
tag_invoke(hpx::for_each_t,
           hpx::kokkos::kokkos_task_policy_shim<Executor, Parameters> policy,
           Iter first, Iter last, F &&f) {
  return detail::for_each_helper(policy.executor().instance(), first, last, f);
}

// For each range overloads for a range
template <typename Range, typename F>
void tag_invoke(hpx::ranges::for_each_t, hpx::kokkos::kokkos_policy policy,
                Range &&r, F &&f) {

  detail::for_each_range_helper(policy.executor().instance(),
                                std::forward<Range>(r), std::forward<F>(f))
      .get();
}

template <typename Executor, typename Parameters, typename Range, typename F>
void tag_invoke(hpx::ranges::for_each_t,
                hpx::kokkos::kokkos_policy_shim<Executor, Parameters> policy,
                Range &&r, F &&f) {

  detail::for_each_range_helper(policy.executor().instance(),
                                std::forward<Range>(r), std::forward<F>(f));
}

template <typename Range, typename F>
hpx::shared_future<void> tag_invoke(hpx::ranges::for_each_t,
                                    hpx::kokkos::kokkos_task_policy policy,
                                    Range &&r, F &&f) {

  return detail::for_each_range_helper(
      policy.executor().instance(), std::forward<Range>(r), std::forward<F>(f));
}

template <typename Executor, typename Parameters, typename Range, typename F>
hpx::shared_future<void>
tag_invoke(hpx::ranges::for_each_t,
           hpx::kokkos::kokkos_task_policy_shim<Executor, Parameters> policy,
           Range &&r, F &&f) {

  return detail::for_each_range_helper(
      policy.executor().instance(), std::forward<Range>(r), std::forward<F>(f));
}
} // namespace kokkos
} // namespace hpx
