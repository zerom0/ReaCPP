#include "Point.h"

#include <cmath>
#include <tuple>

double distance(const Point& from, const Point& to) {
  const auto a = to.x_ - from.x_;
  const auto b = to.y_ - from.y_;
  return sqrt(a*a + b*b);
}

double heading(const Point &from, const Point &to) {
  auto ak = to.x_ - from.x_;
  auto gk = to.y_ - from.y_;
  auto hyp = distance(from, to);
  auto angle = asin(gk/hyp) * 180.0 / M_PI;
  return angle + ((ak >= 0 ) ? ((gk >= 0) ? 0 : 360)
                             : ((gk >= 0) ? 90 : 270));
}

bool operator==(const Point& p1, const Point& p2) {
  return std::make_pair(p1.x_, p1.y_) == std::make_pair(p2.x_, p2.y_);
}

bool operator!=(const Point& p1, const Point& p2) {
  return !(p1 == p2);
}

Point operator+(const Point &p1, const Point &p2) {
  return Point{p1.x_ + p2.x_, p1.y_ + p2.y_};
}

std::ostream& operator<<(std::ostream& ost, const Point& p) {
  ost << "(x=" << p.x_ << ", y=" << p.y_ << ')';
  return ost;
}

Points sortByDistance(const Point& from, const Points& points) {
  auto locations = points;
  locations.sort([from](Point& a, Point& b) {return distance(from, a) < distance(from, b);});
  return locations;
}
std::ostream& operator<<(std::ostream& ost, const Points& ps) {
  for (auto& p : ps) {
    ost << p << ", ";
  }
  return ost;
}
