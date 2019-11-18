#pragma once

#include "DiskVec.hpp"
#include "parser.hpp"
#include "Regex.hpp"

#include "../../lib/unordered_map.hpp"
#include "../../lib/string.hpp"
#include "../../lib/string_view.hpp"

class RobotsTxt {
public:
    static void init() {
        delete ptr;
        ptr = new RobotsTxt();
    }

    static RobotsTxt & getRobots() {
        return *ptr;
    }

    bool canVisit(fb::StringView hostname, const char *loc) {
        auto it = map.find(hostname);
        if (it == map.end()) {
            // get html
        }

        for (auto &regex : it->second)
            if (regex.match(loc))
                return true;

        return it->second.empty();
    }

private:
    RobotsTxt() = default;

    void getRobotsTxt() {
    }

    static RobotsTxt *ptr;
    UnorderedMap<fb::StringView, fb::Vector<Regex>> map;
};
