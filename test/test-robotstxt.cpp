#include <disk/robots.hpp>

#include "doctest.h"

TEST_CASE( "RobotsTxt" ) {
    auto &robots = RobotsTxt::getRobots();

    CHECK( !robots.canVisit("www.google.com"_sv, "/f") );
}
