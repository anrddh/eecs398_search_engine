#pragma once

#include <disk/disk_vec.hpp>
#include <parse/regex.hpp>
#include <http/download_html.hpp>

//#include "DownloadHTML.hpp"

#include <fb/unordered_map.hpp>
#include <fb/string.hpp>
#include <fb/string_view.hpp>
#include <fb/utility.hpp>
#include <fb/view.hpp>

#include <iostream>

class RobotsTxt {
public:
    static RobotsTxt & getRobots() {
        static RobotsTxt robot;
        return robot;
    }

    // "location has to be null-terminated"
    bool canVisit(fb::StringView hostname, fb::StringView location) {
        auto it = robotsParsers.find(hostname);
        if (it == robotsParsers.end()) {
            auto ht = HTTPDownloader();
            getRobotsTxt();
            fb::String str = "http://google.com/robots.txt";
            std::cout << ht.PrintPlainTxt(str).downloadedContent;
            // get robots.txt file
        }

        auto &allowParsers = it->first;
        auto &disallowParsers = it->second;

        for (auto &regex : allowParsers)
            if (regex.match(location.data())) {
                std::cout << "Decided to allow " << location << " while matching\n";
                return true;
            }

        for (auto &regex : disallowParsers)
            if (regex.match(location.data())) {
                std::cout << "Decided to disallow " << location << " while matching\n";
                return false;
            }

        return true;
    }

    fb::StringView getLineFromStr(fb::StringView str) {
        auto pos = str.find('\n');

        if (pos == fb::StringView::npos)
            return {};

        return str.substr(pos + 1);
    }

    void constructRobotParser(fb::StringView hostname, fb::String &robotsTxt) {
        constexpr auto userAgentStr = "User-agent: .*\n"_sv;
        constexpr auto disallowStr = "Disallow: "_sv;
        constexpr auto allowStr = "Allow: "_sv;

        fb::Pair<fb::Vector<Regex>,fb::Vector<Regex>> pair;

        if (robotsTxt.empty()) {
            robotsParsers[hostname] = std::move(pair);
            return;
        }

        auto oneBeforeEnd = robotsTxt.end() - 1;
        for (auto it = robotsTxt.begin(); it != oneBeforeEnd; ++it)
            if (*(it+1) == '*' && *it != '\\') {
                it = robotsTxt.insert(it+1, '.');
                oneBeforeEnd = robotsTxt.end() - 1;
            }

        std::cout << robotsTxt << std::endl;

        fb::StringView robotsTxtView (robotsTxt.data(), robotsTxt.size());
        auto pos = robotsTxtView.find(userAgentStr);
        robotsTxtView.removePrefix(pos + userAgentStr.size());

        if (robotsTxtView.empty() || pos == fb::StringView::npos) {
            robotsParsers[hostname] = std::move(pair);
            return;
        }

        while (true) {
            bool allow = false;
            if (robotsTxtView.front() == allowStr.front()) {
                robotsTxtView.removePrefix(allowStr.size());
                allow = true;
            } else if (robotsTxtView.front() == disallowStr.front())
                robotsTxtView.removePrefix(disallowStr.size());
            else
                break;

            auto pos = robotsTxtView.find('\n');
            if (robotsTxtView.empty() || pos == fb::StringView::npos)
                break;

            robotsTxt[robotsTxtView.data() + pos - robotsTxt.data()] = 0;

            if (allow) {
                std::cout << "Allowing: " << robotsTxtView.data() << '\n';
                pair.first.emplaceBack(robotsTxtView.data());
            } else {
                std::cout << "Disallowing: " << robotsTxtView.data() << '\n';
                pair.second.emplaceBack(robotsTxtView.data());
            }

            robotsTxtView.removePrefix(pos + 1);

            if (robotsTxtView.empty())
                break;
        }

        robotsParsers[hostname] = std::move(pair);
    }

    void getRobotsTxt() {
    }

private:
    RobotsTxt() = default;

    fb::UnorderedMap<fb::StringView, unsigned int> robotsLoc;
    fb::UnorderedMap<fb::StringView,
                 fb::Pair<fb::Vector<Regex>,
                          fb::Vector<Regex>>> robotsParsers;
};
