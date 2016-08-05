/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include "Nop.h"

/**
 * Map<T> manipulates the stream by applies the Function f to its elements.
 */
template<typename F, typename N>
class Map {
 public:
  F function_;
  N next_;

  Map(F f, N n)
      : function_(f)
      , next_(n)
  {
  }

  template<typename T>
  void onNext(T value) {
    next_.onNext(function_(value));
  }

  void onEnd() {
    next_.onEnd();
  }
};


template<typename F>
auto map(F f) {
  return Map<F, Nop<int>>(f, Nop<int>());
}


template<typename F, typename N>
auto map(F f, N n) {
  return Map<F, N>(f, n);
}


template<typename F, typename N>
auto operator|(Map<F, Nop<int>> m, N n) {
  return map(m.function_, n);
}


template<typename F, typename X, typename N>
auto operator|(Map<F, X> m, N n) {
  return map(m.function_, m.next_ | n);
}
