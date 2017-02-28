#pragma once

#include <ostream>

struct Direction {
  double angle_;
  double distance_;
};

struct Point;

Direction getDirection(const Point& from, const Point& to);

std::ostream& operator<<(std::ostream& ost, const Direction& d);