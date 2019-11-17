#pragma once

#include "DiskVec.hpp"

#include "../../lib/unordered_map.hpp"
#include "../../lib/string.hpp"
#include "../../lib/string_view.hpp"

#include "parser.hpp"

class RobotsTxt {
public:
    static void init() {
        delete ptr;
        ptr = new RobotsTxt();
    }

    static RobotsTxt & getRobots() {
        return *ptr;
    }

    bool canVisit(fb::StringView hostname, fb::StringView loc) {
        auto it = map.find(hostname);
        if (it == map.end()) {
            // get html
        }

        if (!it->second)
            return true;
    }

private:
    RobotsTxt() = default;

    void getRobotsTxt() {
    }

    static RobotsTxt *ptr;
    UnorderedMap<fb::StringView, unsigned int> map;
};
