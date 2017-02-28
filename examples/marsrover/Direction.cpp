#include "Direction.h"

#include "Point.h"

Direction getDirection(const Point& from, const Point& to) {
  return Direction{.distance_=distance(from, to), .angle_=heading(from, to)};
}

std::ostream& operator<<(std::ostream& ost, const Direction& d) {
  ost << "(a=" << d.angle_ << "°, d=" << d.distance_ << ')';
  return ost;
}
