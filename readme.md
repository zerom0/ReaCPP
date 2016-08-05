# ReaCPP

ReaCPP is an extensible header only C++ library for reactive processing of data. This allows for example to implicitly process messages arriving at arbitrary points in time through a network interface.

The library provides primitive functions that can be extended and combined into more complex functions.

## Introduction

The reactive design pattern can be seen as an inversion of the iterator pattern.

With the iterator pattern one is iterating over elements of a collection or data from a data source like a database, network interface or broker until the end is reached or an error occured.

Example of iterating over a data source, e.g. a container:

    auto it = source.begin();
    while (it != source.end()) {
      // network interface or database connections could block for the next element
      auto raw_data = it++;
      auto processed_data = process(raw_data);
      if (predicate(processed_data)) {
        output(processed_data);
      }
    }

The reactive pattern processes the events onNext, onEnd and onError asynchronously. Well designed reactive components are reusable and composable.

Interface of a reactive component:

    class ReactiveComponent {
    public:
      void onNext();  // process the data
      void onEnd();   // handle the end of the data stream
      void onError(); // handle an error with the data stream
    };

Example of a composed reactive processing chain from the data source though the processing and filtering steps up to the output.

    source >> map(process) | filter(predicate) | output;


## Library

The library provides the basic reactive components like map, filter and reduce for building processing chains as well as the needed operators for joining those elements.

Class names start with a capital letter, whereas factory functions to create objects use the same name but start with a lowercase letter. Those factory functions allow for dynamic deduction of the classes template parameters.


### Map : F -> A -> B

Map allows the application of a function to each element that is being processed. Additionally Map can transform the input type into a new output type, e.g. int to string.

The following example works on input values of type int and squares each input value for the next reactive component in the processing chain.

    auto square = map([](int v){ return v * v; });

The following example does not modify the value but prints it as a side effect to stdout.

    auto output = map([](int v){ cout << v; return v; });


### Filter : F -> A -> A

Filter forwards only input values that fulfil the given predicate and drops all other values from further processing.

The following example works on input values of type int and forwards only even numbers to the next reactive component in the chain.

    auto keep_even = filter([](int v){ return (v%2) == 0; });


### Reduce : F -> A -> B

Reduce combines all received elements into one final value by combining each newly received value with the previous received values represented by the accumulator.

The following example works on input values of type int and sums them up until the onEnd() event is received. Then it forwards the combined value to the next reactive component in the chain.

    auto sum = reduce(0, [](int acc, int v){ return acc + v; });


### Joining operator |

This library provides the operator | to combine reactive components into processing chains. This operator can only join components when the output type of the first component matches the input type of the second component.

The following example combines the above shown examples and creates a processing chain that outputs the sum of even squares of all input values.

    auto sum_of_even_squares = square | keep_even | sum | output;
