#include <disk/robots.hpp>

#include "doctest.h"

TEST_CASE( "RobotsTxt Test 1" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Disallow: /fish\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( ! robots.canVisit("test.com", "/fish") );
    CHECK( ! robots.canVisit("test.com", "/fish.html") );
    CHECK( ! robots.canVisit("test.com", "/fish.html") );
    CHECK( ! robots.canVisit("test.com", "/fish/salmon.html") );
    CHECK( ! robots.canVisit("test.com", "/fishheads") );
    CHECK( ! robots.canVisit("test.com", "/fishheads/yummy.html") );
    CHECK( ! robots.canVisit("test.com", "/fish.php?id=anything") );
    CHECK( robots.canVisit("test.com", "/Fish.asp") );
    CHECK( robots.canVisit("test.com", "/catfish") );
    CHECK( robots.canVisit("test.com", "/?id=fish") );
}

TEST_CASE( "RobotsTxt Test 2" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Allow: /fish\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( robots.canVisit("test.com", "/fish") );
    CHECK( robots.canVisit("test.com", "/fish.html") );
    CHECK( robots.canVisit("test.com", "/fish/salmon.html") );
    CHECK( robots.canVisit("test.com", "/fishheads") );
    CHECK( robots.canVisit("test.com", "/fishheads/yummy.html") );
    CHECK( robots.canVisit("test.com", "/fish.php?id=anything") );
    CHECK( robots.canVisit("test.com", "/Fish.asp") );
    CHECK( robots.canVisit("test.com", "/catfish") );
    CHECK( robots.canVisit("test.com", "/?id=fish") );
}

TEST_CASE( "RobotsTxt Test 3" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Disallow: /fish*\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( ! robots.canVisit("test.com", "/fish") );
    CHECK( ! robots.canVisit("test.com", "/fish.html") );
    CHECK( ! robots.canVisit("test.com", "/fish/salmon.html") );
    CHECK( ! robots.canVisit("test.com", "/fishheads") );
    CHECK( ! robots.canVisit("test.com", "/fishheads/yummy.html") );
    CHECK( ! robots.canVisit("test.com", "/fish.php?id=anything") );
    CHECK( robots.canVisit("test.com", "/Fish.asp") );
    CHECK( robots.canVisit("test.com", "/catfish") );
    CHECK( robots.canVisit("test.com", "/?id=fish") );
}

TEST_CASE( "RobotsTxt Test 4" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Allow: /fish*\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( robots.canVisit("test.com", "/fish") );
    CHECK( robots.canVisit("test.com", "/fish.html") );
    CHECK( robots.canVisit("test.com", "/fish/salmon.html") );
    CHECK( robots.canVisit("test.com", "/fishheads") );
    CHECK( robots.canVisit("test.com", "/fishheads/yummy.html") );
    CHECK( robots.canVisit("test.com", "/fish.php?id=anything") );
    CHECK( robots.canVisit("test.com", "/Fish.asp") );
    CHECK( robots.canVisit("test.com", "/catfish") );
    CHECK( robots.canVisit("test.com", "/?id=fish") );
}

TEST_CASE( "RobotsTxt Test 5" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Disallow: /fish/\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( ! robots.canVisit("test.com", "/fish/") );
    CHECK( ! robots.canVisit("test.com", "/fish/?id=anything") );
    CHECK( ! robots.canVisit("test.com", "/fish/salmon.html") );
    CHECK( robots.canVisit("test.com", "/fish") );
    CHECK( robots.canVisit("test.com", "/fish.html") );
    CHECK( robots.canVisit("test.com", "/Fish/Salmon.asp") );
}

TEST_CASE( "RobotsTxt Test 6" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Allow: /fish/\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( robots.canVisit("test.com", "/fish/") );
    CHECK( robots.canVisit("test.com", "/fish/?id=anything") );
    CHECK( robots.canVisit("test.com", "/fish/salmon.html") );
    CHECK( robots.canVisit("test.com", "/fish") );
    CHECK( robots.canVisit("test.com", "/fish.html") );
    CHECK( robots.canVisit("test.com", "/Fish/Salmon.asp") );
}

TEST_CASE( "RobotsTxt Test 7" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Disallow: /*.php\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( ! robots.canVisit("test.com", "/filename.php") );
    CHECK( ! robots.canVisit("test.com", "/folder/filename.php") );
    CHECK( ! robots.canVisit("test.com", "/folder/filename.php?parameters") );
    CHECK( ! robots.canVisit("test.com", "/folder/any.php.file.html") );
    CHECK( ! robots.canVisit("test.com", "/filename.php/") );
    CHECK( robots.canVisit("test.com", "/") );
    CHECK( robots.canVisit("test.com", "/windows.PHP") );
}

TEST_CASE( "RobotsTxt Test 8" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Allow: /*.php\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( robots.canVisit("test.com", "/filename.php") );
    CHECK( robots.canVisit("test.com", "/folder/filename.php") );
    CHECK( robots.canVisit("test.com", "/folder/filename.php?parameters") );
    CHECK( robots.canVisit("test.com", "/folder/any.php.file.html") );
    CHECK( robots.canVisit("test.com", "/filename.php/") );
    CHECK( robots.canVisit("test.com", "/") );
    CHECK( robots.canVisit("test.com", "/windows.PHP") );
}


TEST_CASE( "RobotsTxt Test 9" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Disallow: /*.php$\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( ! robots.canVisit("test.com", "/filename.php") );
    CHECK( ! robots.canVisit("test.com", "/folder/filename.php") );
    CHECK( robots.canVisit("test.com", "/folder/filename.php?parameters") );
    CHECK( robots.canVisit("test.com", "/folder/any.php.file.html") );
    CHECK( robots.canVisit("test.com", "/filename.php/") );
    CHECK( robots.canVisit("test.com", "/") );
    CHECK( robots.canVisit("test.com", "/windows.PHP") );
}

TEST_CASE( "RobotsTxt Test 10" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Allow: /*.php$\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( robots.canVisit("test.com", "/filename.php") );
    CHECK( robots.canVisit("test.com", "/folder/filename.php") );
    CHECK( robots.canVisit("test.com", "/folder/filename.php?parameters") );
    CHECK( robots.canVisit("test.com", "/folder/any.php.file.html") );
    CHECK( robots.canVisit("test.com", "/filename.php/") );
    CHECK( robots.canVisit("test.com", "/") );
    CHECK( robots.canVisit("test.com", "/windows.PHP") );
}

TEST_CASE( "RobotsTxt Test 11" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Disallow: /fish*.php\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( ! robots.canVisit("test.com", "/fish.php") );
    CHECK( ! robots.canVisit("test.com", "/fishheads/catfish.php?parameters") );
    CHECK( robots.canVisit("test.com", "/Fish.PHP") );
}


TEST_CASE( "RobotsTxt Test 12" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Allow: /fish*.php\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( robots.canVisit("test.com", "/fish.php") );
    CHECK( robots.canVisit("test.com", "/fishheads/catfish.php?parameters") );
    CHECK( robots.canVisit("test.com", "/Fish.PHP") );
}


TEST_CASE( "RobotsTxt Test 13" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Allow: /p\n\
Disallow: /\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( robots.canVisit("test.com", "/page") );
}

TEST_CASE( "RobotsTxt Test 14" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Allow: /folder\n\
Disallow: /folder\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( robots.canVisit("test.com", "/folder/page") );
}

TEST_CASE( "RobotsTxt Test 15" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Allow: /page\n\
Disallow: /*.htm\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( robots.canVisit("test.com", "/page.htm") );
}

TEST_CASE( "RobotsTxt Test 16" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Allow: /$\n\
Disallow: /\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( robots.canVisit("test.com", "/") );
}

TEST_CASE( "RobotsTxt Test 17" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Allow: /$\n\
Disallow: /\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( ! robots.canVisit("test.com", "/page.htm") );
}
