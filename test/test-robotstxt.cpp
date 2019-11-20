#include <disk/robots.hpp>

#include "doctest.h"

#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>

//TEST_CASE( "RobotsTxt" ) {
//    auto &robots = RobotsTxt::getRobots();
//
//    std::ifstream t("/Users/anrddh/google_robots.txt");
//    std::string str((std::istreambuf_iterator<char>(t)),
//                    std::istreambuf_iterator<char>());
//
//    fb::String googleRobots(str.data(), str.size());
//
//    robots.constructRobotParser("www.google.com", googleRobots);
//
//    CHECK( robots.canVisit("www.google.com"_sv, "/?hl=") );
//    CHECK( ! robots.canVisit("www.google.com"_sv, "/purchases") );
//}

TEST_CASE( "RobotsTxt Test 1" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Disallow: /fish\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( ! robots.canVisit("test.com"_sv, "/fish") );
    CHECK( ! robots.canVisit("test.com"_sv, "/fish.html") );
    CHECK( ! robots.canVisit("test.com"_sv, "/fish/salmon.html") );
    CHECK( ! robots.canVisit("test.com"_sv, "/fishheads") );
    CHECK( ! robots.canVisit("test.com"_sv, "/fishheads/yummy.html") );
    CHECK( ! robots.canVisit("test.com"_sv, "/fish.php?id=anything") );
    CHECK( robots.canVisit("test.com"_sv, "/Fish.asp") );
    CHECK( robots.canVisit("test.com"_sv, "/catfish") );
    CHECK( robots.canVisit("test.com"_sv, "/?id=fish") );
}

TEST_CASE( "RobotsTxt Test 2" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Allow: /fish\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( robots.canVisit("test.com"_sv, "/fish") );
    CHECK( robots.canVisit("test.com"_sv, "/fish.html") );
    CHECK( robots.canVisit("test.com"_sv, "/fish/salmon.html") );
    CHECK( robots.canVisit("test.com"_sv, "/fishheads") );
    CHECK( robots.canVisit("test.com"_sv, "/fishheads/yummy.html") );
    CHECK( robots.canVisit("test.com"_sv, "/fish.php?id=anything") );
    CHECK( robots.canVisit("test.com"_sv, "/Fish.asp") );
    CHECK( robots.canVisit("test.com"_sv, "/catfish") );
    CHECK( robots.canVisit("test.com"_sv, "/?id=fish") );
}

TEST_CASE( "RobotsTxt Test 3" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Disallow: /fish*\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( ! robots.canVisit("test.com"_sv, "/fish") );
    CHECK( ! robots.canVisit("test.com"_sv, "/fish.html") );
    CHECK( ! robots.canVisit("test.com"_sv, "/fish/salmon.html") );
    CHECK( ! robots.canVisit("test.com"_sv, "/fishheads") );
    CHECK( ! robots.canVisit("test.com"_sv, "/fishheads/yummy.html") );
    CHECK( ! robots.canVisit("test.com"_sv, "/fish.php?id=anything") );
    CHECK( robots.canVisit("test.com"_sv, "/Fish.asp") );
    CHECK( robots.canVisit("test.com"_sv, "/catfish") );
    CHECK( robots.canVisit("test.com"_sv, "/?id=fish") );
}

TEST_CASE( "RobotsTxt Test 4" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Allow: /fish*\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( robots.canVisit("test.com"_sv, "/fish") );
    CHECK( robots.canVisit("test.com"_sv, "/fish.html") );
    CHECK( robots.canVisit("test.com"_sv, "/fish/salmon.html") );
    CHECK( robots.canVisit("test.com"_sv, "/fishheads") );
    CHECK( robots.canVisit("test.com"_sv, "/fishheads/yummy.html") );
    CHECK( robots.canVisit("test.com"_sv, "/fish.php?id=anything") );
    CHECK( robots.canVisit("test.com"_sv, "/Fish.asp") );
    CHECK( robots.canVisit("test.com"_sv, "/catfish") );
    CHECK( robots.canVisit("test.com"_sv, "/?id=fish") );
}

TEST_CASE( "RobotsTxt Test 5" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Disallow: /fish/\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( ! robots.canVisit("test.com"_sv, "/fish/") );
    CHECK( ! robots.canVisit("test.com"_sv, "/fish/?id=anything") );
    CHECK( ! robots.canVisit("test.com"_sv, "/fish/salmon.html") );
    CHECK( robots.canVisit("test.com"_sv, "/fish") );
    CHECK( robots.canVisit("test.com"_sv, "/fish.html") );
    CHECK( robots.canVisit("test.com"_sv, "/Fish/Salmon.asp") );
}

TEST_CASE( "RobotsTxt Test 6" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Allow: /fish/\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( robots.canVisit("test.com"_sv, "/fish/") );
    CHECK( robots.canVisit("test.com"_sv, "/fish/?id=anything") );
    CHECK( robots.canVisit("test.com"_sv, "/fish/salmon.html") );
    CHECK( robots.canVisit("test.com"_sv, "/fish") );
    CHECK( robots.canVisit("test.com"_sv, "/fish.html") );
    CHECK( robots.canVisit("test.com"_sv, "/Fish/Salmon.asp") );
}

TEST_CASE( "RobotsTxt Test 7" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Disallow: /*.php\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( ! robots.canVisit("test.com"_sv, "/filename.php") );
    CHECK( ! robots.canVisit("test.com"_sv, "/folder/filename.php") );
    CHECK( ! robots.canVisit("test.com"_sv, "/folder/filename.php?parameters") );
    CHECK( ! robots.canVisit("test.com"_sv, "/folder/any.php.file.html") );
    CHECK( ! robots.canVisit("test.com"_sv, "/filename.php/") );
    CHECK( robots.canVisit("test.com"_sv, "/") );
    CHECK( robots.canVisit("test.com"_sv, "/windows.PHP") );
}

TEST_CASE( "RobotsTxt Test 8" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Allow: /*.php\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( robots.canVisit("test.com"_sv, "/filename.php") );
    CHECK( robots.canVisit("test.com"_sv, "/folder/filename.php") );
    CHECK( robots.canVisit("test.com"_sv, "/folder/filename.php?parameters") );
    CHECK( robots.canVisit("test.com"_sv, "/folder/any.php.file.html") );
    CHECK( robots.canVisit("test.com"_sv, "/filename.php/") );
    CHECK( robots.canVisit("test.com"_sv, "/") );
    CHECK( robots.canVisit("test.com"_sv, "/windows.PHP") );
}


TEST_CASE( "RobotsTxt Test 9" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Disallow: /*.php$\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( ! robots.canVisit("test.com"_sv, "/filename.php") );
    CHECK( ! robots.canVisit("test.com"_sv, "/folder/filename.php") );
    CHECK( robots.canVisit("test.com"_sv, "/folder/filename.php?parameters") );
    CHECK( robots.canVisit("test.com"_sv, "/folder/any.php.file.html") );
    CHECK( robots.canVisit("test.com"_sv, "/filename.php/") );
    CHECK( robots.canVisit("test.com"_sv, "/") );
    CHECK( robots.canVisit("test.com"_sv, "/windows.PHP") );
}

TEST_CASE( "RobotsTxt Test 10" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Allow: /*.php$\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( robots.canVisit("test.com"_sv, "/filename.php") );
    CHECK( robots.canVisit("test.com"_sv, "/folder/filename.php") );
    CHECK( robots.canVisit("test.com"_sv, "/folder/filename.php?parameters") );
    CHECK( robots.canVisit("test.com"_sv, "/folder/any.php.file.html") );
    CHECK( robots.canVisit("test.com"_sv, "/filename.php/") );
    CHECK( robots.canVisit("test.com"_sv, "/") );
    CHECK( robots.canVisit("test.com"_sv, "/windows.PHP") );
}

TEST_CASE( "RobotsTxt Test 11" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Disallow: /fish*.php\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( ! robots.canVisit("test.com"_sv, "/fish.php") );
    CHECK( ! robots.canVisit("test.com"_sv, "/fishheads/catfish.php?parameters") );
    CHECK( robots.canVisit("test.com"_sv, "/Fish.PHP") );
}


TEST_CASE( "RobotsTxt Test 12" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Allow: /fish*.php\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( robots.canVisit("test.com"_sv, "/fish.php") );
    CHECK( robots.canVisit("test.com"_sv, "/fishheads/catfish.php?parameters") );
    CHECK( robots.canVisit("test.com"_sv, "/Fish.PHP") );
}


TEST_CASE( "RobotsTxt Test 13" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Allow: /p\n\
Disallow: /\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( robots.canVisit("test.com"_sv, "/page") );
}

TEST_CASE( "RobotsTxt Test 14" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Allow: /folder\n\
Disallow: /folder\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( robots.canVisit("test.com"_sv, "/folder/page") );
}

TEST_CASE( "RobotsTxt Test 15" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Allow: /page\n\
Disallow: /*.htm\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( robots.canVisit("test.com"_sv, "/page.htm") );
}

TEST_CASE( "RobotsTxt Test 16" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Allow: /$\n\
Disallow: /\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( robots.canVisit("test.com"_sv, "/") );
}

TEST_CASE( "RobotsTxt Test 17" ) {
    auto &robots = RobotsTxt::getRobots();

    fb::String robots1 = "User-agent: *\n\
Allow: /$\n\
Disallow: /\n";

    robots.constructRobotParser("test.com", robots1);
    CHECK( ! robots.canVisit("test.com"_sv, "/page.htm") );
}
