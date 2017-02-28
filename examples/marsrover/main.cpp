#include "Direction.h"
#include "Point.h"

#include <Filter.h>
#include <Insert.h>
#include <Map.h>

#include <cmath>
#include <iostream>
#include <list>
#include <unistd.h>

using namespace std;

enum VariantType {
  Nothing,
  CurrentPosition,
  GoodsPositions,
  NextGoodPosition,
  TravelDirection,
};

using Variant = std::tuple<VariantType, Point, Points, Direction>;

/**
 * Filter the event stream for changes in position and list of goods and return
 * the next goods' location if it changed.
 */
auto nextGoody = map([](auto& v) {
  static auto lastPosition = Point{0.0, 0.0};
  static auto lastGoody = Points{};
  static auto prevNextGood = Point{0.0, 0.0};
  auto newNextGoody = prevNextGood;

  switch(get<VariantType>(v)) {
    case CurrentPosition:
      if (lastPosition != get<Point>(v)) {
        lastPosition = get<Point>(v);
        if (!lastGoody.empty()) {
          newNextGoody = sortByDistance(lastPosition, lastGoody).front();
        }
      }
      break;

    case GoodsPositions:
      if (get<Points>(v) == lastGoody) {
        return std::make_tuple(Nothing, Point{}, Points{}, Direction{});
      } else {
        lastGoody = get<Points>(v);
        newNextGoody = sortByDistance(lastPosition, lastGoody).front();
      }
      break;

    default:
      break;
  }

  if (newNextGoody == prevNextGood) {
    return std::make_tuple(Nothing, Point{}, Points{}, Direction{});
  } else {
    prevNextGood = newNextGoody;
    return std::make_tuple(NextGoodPosition, newNextGoody, Points{}, Direction{});
  }
});

/**
 * Monitor the last position and the next goody's position and derive the direction
 * to travel for reaching the good.
 */
auto direction = map([](auto& v) {
  static auto lastPosition = Point{0.0, 0.0};
  static auto lastGoody = Point{0.0, 0.0};
  switch(get<VariantType>(v)) {
    case CurrentPosition:
      lastPosition = get<Point>(v);
      if (lastPosition != lastGoody) {
        // Position has changed, so we enter the new travel directions
        return std::make_tuple(TravelDirection, Point{}, Points{}, getDirection(lastPosition, lastGoody));
      }

    case NextGoodPosition:
      // The next good is at a different position, so we enter the new travel directions
      lastGoody = get<Point>(v);
      return std::make_tuple(TravelDirection, Point{}, Points{}, getDirection(lastPosition, lastGoody));

    default:
      break;
  }

  // Nothing changed, so we stay with the old travel directions
  return std::make_tuple(Nothing, Point{}, Points{}, Direction{});
});

/**
 * Filter the event stream by removing Nops.
 */
auto dropNops = filter([](auto& v) {
  switch(get<VariantType>(v)) {
    case Nothing:
      return false;
    default:
      return true;
  }
});

auto currentDirection = Direction{.angle_=0, .distance_=0};

/**
 * Finally process the events by either printing them or setting the directions
 * for the rover to follow.
 */
auto process = map([](auto& v) {
  switch(get<VariantType>(v)) {
    case CurrentPosition:
      cout << "Position: " << get<Point>(v) << endl;
      break;
    case GoodsPositions:
      cout << "Goods: " << get<Points>(v) << endl;
      break;
    case NextGoodPosition:
      cout << "NextGoods: " << get<Point>(v) << endl;
      break;
    case TravelDirection:
      currentDirection = get<Direction>(v);
      cout << "Move: " << currentDirection << endl;
      break;
    default:
      break;
  }
  return 0;
});

/**
 * Travel from a start point into direction by distance and return the new location.
 */
Point travel(Point start, Direction direction, int distance) {
  return start + Point{.x_=distance*cos(direction.angle_*M_PI/180), .y_=distance*sin(direction.angle_*M_PI/180)};
};

/**
 * Test if point is very close to any point in the list.
 */
bool goodsReached(Points list, Point point) {
  return find_if(begin(list), end(list), [point](const auto& p){ return distance(point, p) < 1;}) != list.end();
}

/**
 * Return new list of points without the points that are very close to point.
 */
auto dropGood(Points list, Point point) {
  Points newPoints;
  copy_if(begin(list), end(list), back_inserter(newPoints), [point](const Point& p){ return distance(point, p) >= 1;});
  return newPoints;
}

int main() {
  auto goodsToCollect = Points{Point{.x_=75, .y_=75},
                               Point{.x_=75, .y_=50},
                               Point{.x_=100, .y_=100},
                               Point{.x_=50, .y_=100},
                               Point{.x_=50, .y_=10},
                               Point{.x_=100, .y_=50}};

  auto currentPosition = Point{.x_=50, .y_=50};

  std::list<Variant> events = {
      std::make_tuple(CurrentPosition, currentPosition, Points{}, Direction{}),
      std::make_tuple(GoodsPositions, Point{}, goodsToCollect, Direction{}),
  };

  auto eventProcessor = insert(nextGoody) | insert(direction) | dropNops | process;

  // Lets only make a finite amount of steps before the batteries run empty
  for (auto i = 0; i < 100; ++i) {
    // Feed the events to the processor
    while (not events.empty()) {
      eventProcessor.onNext(events.front());
      events.pop_front();
    }

    // Sleep for humans to follow
    usleep(200'000);

    // Let the rover travel
    currentPosition = travel(currentPosition, currentDirection, 1);
    events.emplace_back(std::make_tuple(CurrentPosition, currentPosition, Points{}, Direction{}));

    // If the rover hit a goody we update the list of goods
    if (goodsReached(goodsToCollect, currentPosition)) {
      goodsToCollect = dropGood(goodsToCollect, currentPosition);
      events.emplace_back(std::make_tuple(GoodsPositions, Point{}, goodsToCollect, Direction{}));
    }
  }
}
