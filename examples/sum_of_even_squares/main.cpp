#include <Filter.h>
#include <Map.h>
#include <Reduce.h>

#include <iostream>
#include <vector>

using namespace std;

/**
 * The operator >> streams the values from the container into the reactive chain.
 *
 * @param container Container with values
 * @param reactive_chain Reactive chain that processes the values
 *
 * @warning The reactive chain must be previously combined into one symbol or
 *          grouped by parenthesis, otherwise only the first element of the chain
 *          processes the values.
 *
 * @example container >> (map(...) | filter(...) | reduce(...));
 */
template<typename T, typename R>
void operator >> (T& container, R& reactiveChain) {
  for (auto& value : container) reactiveChain.onNext(value);
  reactiveChain.onEnd();
};

int main() {
  auto square = map([](int v){ return v * v; });
  auto keep_even = filter([](int v){ return (v % 2) == 0; });
  auto sum = reduce(0, [](int acc, int v){ return acc + v; });
  auto output = map([](int v){ cout << v; return 0; });
  auto sum_of_even_squares = square | keep_even | sum | output;

  vector<int> values = {1, 3, 4, 2, 7, 6};

  values >> sum_of_even_squares;
};