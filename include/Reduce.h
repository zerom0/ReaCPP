/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include "Nop.h"

/**
 * Reduces the stream to one value (fold left).
 */
template<typename T, typename F, typename N>
class Reduce {
 public:
  F f_;
  N next_;
  T acc_;

  Reduce(T acc, const F& f, N n)
      : acc_(acc)
      , f_(f)
      , next_(n)
  {
  }

  void onNext(const T& value) {
    acc_ = f_(acc_, value);
  }

  void onEnd() {
    next_.onNext(acc_);
    next_.onEnd();
  }
};


template<typename T, typename F>
auto reduce(T acc, const F& f) {
  return Reduce<T, F, Nop<int>>(acc, f, Nop<int>());
}


template<typename T, typename F, typename N>
auto reduce(T acc, const F& f, N n) {
  return Reduce<T, F, N>(acc, f, n);
}


template<typename T, typename F, typename N>
auto operator|(Reduce<T, F, Nop<int>> r, N n) {
  return reduce(r.acc_, r.f_, n);
}


template<typename T, typename F, typename X, typename N>
auto operator|(Reduce<T, F, X> r, N n) {
  return reduce(r.acc_, r.f_, r.next_ | n);
}
