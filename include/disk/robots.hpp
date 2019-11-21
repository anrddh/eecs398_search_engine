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

constexpr auto Err404 = "404"_sv;

template <typename Hostname = fb::String>
class RobotsImpl {
public:
    using HostnameType = Hostname;

    static RobotsImpl & getRobots() {
        static RobotsImpl robot;
        return robot;
    }

    bool canVisit(HostnameType hostname, fb::StringView location) {
        return canVisitImpl(hostname, location) & 1;
    }

    // "location has to be null-terminated"
    char canVisitImpl(const HostnameType &hostname, fb::StringView location) {
        int to_return = 0;

        auto it = robotsParsers.find(hostname);
        if (it == robotsParsers.end()) {
            ++to_return;
            to_return <<= 1;
            try {
                HTTPDownloader ht;
                auto file = ht.PrintPlainTxt(hostname + location);
                constructRobotParser(hostname, file);
            } catch(const ConnectionException &e) {
                if (e.msg == Err404)
                    robotsParsers[hostname];

                return ++to_return;
            }
        }

        auto &allowParsers = it->first;
        auto &disallowParsers = it->second;

        for (auto &regex : allowParsers)
            if (regex.match(location.data()))
                return ++to_return;

        for (auto &regex : disallowParsers)
            if (regex.match(location.data()))
                return to_return;

        return ++to_return;
    }

    fb::StringView getLineFromStr(fb::StringView str) {
        auto pos = str.find('\n');

        if (pos == fb::StringView::npos)
            return {};

        return str.substr(pos + 1);
    }

    void constructRobotParser(const HostnameType &hostname, fb::String &robotsTxt) {
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


            try {
                if (allow)
                    pair.first.emplaceBack(robotsTxtView.data());
                else
                    pair.second.emplaceBack(robotsTxtView.data());
            } catch(Regex::Error &e) {
                std::cerr << "Could not construct regular expression for: robotsTxtView.data()\n";
                std::cerr << "Error: " << e.what() << '\n';
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
    RobotsImpl() = default;

    fb::UnorderedMap<HostnameType,
                     fb::Pair<fb::Vector<Regex>,
                              fb::Vector<Regex>>> robotsParsers;
};

using RobotsTxt = RobotsImpl<>;
