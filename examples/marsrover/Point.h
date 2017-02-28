#pragma once

#include <list>
#include <ostream>

struct Point {
  double x_;
  double y_;
};

using Points = std::list<Point>;

double distance(const Point& from, const Point& to);

double heading(const Point &from, const Point &to);

bool operator==(const Point& p1, const Point& p2);

bool operator!=(const Point& p1, const Point& p2);

Point operator+(const Point& p1, const Point& p2);

std::ostream& operator<<(std::ostream& ost, const Point& p);

Points sortByDistance(const Point& from, const Points& points);

std::ostream& operator<<(std::ostream& ost, const Points& ps);