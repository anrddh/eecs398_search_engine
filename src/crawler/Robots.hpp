#pragma once

#include "DiskVec.hpp"
#include "regex.hpp"
//#include "DownloadHTML.hpp"

#include "../../lib/unordered_map.hpp"
#include "../../lib/string.hpp"
#include "../../lib/string_view.hpp"
#include "../../lib/utility.hpp"

class RobotsTxt {
public:
    static void init( fb::StringView filename, bool init ) {
        delete ptr;
        ptr = new RobotsTxt(filename, init);
    }

    static RobotsTxt & getRobots() {
        return *ptr;
    }

    bool canVisit(fb::StringView hostname, const char *loc) {
        auto it = robotsParsers.find(hostname);
        if (it == robotsParsers.end()) {
            getRobotsTxt();
            // get robots.txt file
        }

        auto &allowParsers = it->first;
        auto &disallowParsers = it->second;

        for (auto &regex : allowParsers)
            if (regex.match(loc))
                return true;

        for (auto &regex : disallowParsers)
            if (regex.match(loc))
                return false;

        return allowParsers.empty() && disallowParsers.empty();
    }

    RobotsTxt(fb::StringView filename, bool init) : robots(filename, init) {}

    fb::StringView getLineFromStr(fb::StringView str) {
        auto pos = str.find('\n');

        if (pos == fb::StringView::npos)
            return {};

        return str.substr(pos + 1);
    }

    static fb::Pair<fb::Vector<Regex>,fb::Vector<Regex>>
    constructRobotParser(fb::StringView robotsTxt) {
        constexpr auto userAgentStr = "User-agent: *\n"_sv;
        constexpr auto disallowStr = "Disallow: "_sv;
        constexpr auto allowStr = "Allow: "_sv;

        fb::Pair<fb::Vector<Regex>,fb::Vector<Regex>> pair;

        auto pos = robotsTxt.find("U"_sv);
        robotsTxt.removePrefix(pos + userAgentStr.size());

        if (robotsTxt.empty() || pos == fb::StringView::npos)
            return pair;

        while (true) {
            bool allow = false;
            if (robotsTxt.front() == allowStr.front()) {
                robotsTxt.removePrefix(allowStr.size());
                allow = true;
            } else if (robotsTxt.front() == disallowStr.front())
                robotsTxt.removePrefix(disallowStr.size());
            else
                break;

            auto pos = robotsTxt.find('\n');
            if (robotsTxt.empty() || pos == fb::StringView::npos)
                break;

            *((char *) robotsTxt.data() + pos) = '\0';

            if (allow)
                pair.first.emplaceBack(robotsTxt.data());
            else
                pair.second.emplaceBack(robotsTxt.data());

            robotsTxt.removePrefix(pos + 1);

            if (robotsTxt.empty())
                break;
        }

        return pair;
    }

    void getRobotsTxt() {
    }

    static RobotsTxt *ptr;
    fb::UnorderedMap<fb::StringView, unsigned int> robotsLoc;
    fb::UnorderedMap<fb::StringView,
                 fb::Pair<fb::Vector<Regex>,
                          fb::Vector<Regex>>> robotsParsers;
    DiskVec<char> robots;
};
