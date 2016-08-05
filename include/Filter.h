/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include "Nop.h"

/**
 * Filter<T> removes all elements from the stream that do not fulfil the predicate.
 */
template<typename P, typename N>
class Filter {
 public:
  P predicate_;
  N next_;

  Filter(const P& p, N n)
      : predicate_(p)
      , next_(n)
  {
  }

  template<typename T>
  void onNext(const T& value) {
    if (predicate_(value)) {
      next_.onNext(value);
    }
  }

  void onEnd() {
    next_.onEnd();
  }
};


template<typename F>
auto filter(const F& f) {
  return Filter<F, Nop<int>>(f, Nop<int>());
}


template<typename F, typename N>
auto filter(const F& f, N n) {
  return Filter<F, N>(f, n);
}


template<typename P, typename N>
auto operator|(Filter<P, Nop<int>> f, N n) {
  return filter(f.predicate_, n);
}


template<typename P, typename X, typename N>
auto operator|(Filter<P, X> f, N n) {
  return filter(f.predicate_, f.next_ | n);
}
