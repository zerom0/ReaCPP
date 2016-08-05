#include "Filter.h"
#include "Map.h"
#include "Reduce.h"

#include <iostream>

/**
 * Prints the commandline options of the program
 */
void printUsageAndExit() {
  std::cout << "Usage: greap <text>\n";
  exit(1);
}

/**
 * This is a simple demo application showing the usage of the reactive map reduce library by
 * implementing the grep function that shows only lines of the input stream that contain the
 * given pattern.
 */
int main(int argc, char* argv[]) {
  std::string searchText = "";

  switch (argc) {
    case 2:
      searchText = argv[1];
      break;
    default:
      printUsageAndExit();
  }

  std::string currentLine;
  unsigned lineNumber = 0;

  auto addLineNumber =
      map([&lineNumber](std::string s){ return std::make_pair(++lineNumber, s); });

  auto keepOnlyMatches =
      filter([searchText](const std::pair<unsigned, std::string>& p){
        return p.second.find(searchText) != std::string::npos;
      });

  auto printLine =
      map([](std::pair<unsigned, std::string> p){
        std::cout << p.first << " : " << p.second << std::endl; return 1;
      });

  auto countLines =
      reduce(0, [](int acc, int v){ return acc + v; });

  auto printSummary =
      map([](int v){ std::cout << "Found " << v << " matches." << std::endl; return 0; });

  auto lineProcessor =
      addLineNumber
      | keepOnlyMatches
      | printLine
      | countLines
      | printSummary;

  while (std::getline(std::cin, currentLine)) {
    lineProcessor.onNext(currentLine);
  }
  lineProcessor.onEnd();
}
