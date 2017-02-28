/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include "Nop.h"

/**
 * Insert<T> manipulates the stream by forking the input stream and applying it to both I and N
 *
 *  --->+------>+---> N
 *       \     /
 *        ->I->
 */
template<typename I, typename N>
class Insert {
 public:
  I insert_;
  N next_;

  Insert(I i, N n)
      : insert_(i)
      , next_(n)
  {
  }

  template<typename T>
  void onNext(T value) {
    next_.onNext(value);
    insert_.onNext(value);
  }

  void onEnd() {
    insert_.onEnd();
  }
};


template<typename I>
auto insert(I i) {
  return Insert<I, Nop<int>>(i, Nop<int>());
}


template<typename I, typename N>
auto insert(I i, N n) {
  return Insert<I, N>(i, n);
}


template<typename I, typename N>
auto operator|(Insert<I, Nop<int>> m, N n) {
  return insert(m.insert_ | n, n);
}


template<typename I, typename X, typename N>
auto operator|(Insert<I, X> m, N n) {
  return insert(m.insert_ | n, m.next_ | n);
}
