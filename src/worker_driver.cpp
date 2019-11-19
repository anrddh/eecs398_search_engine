#include <fb/stddef.hpp>

#include <exception>

using fb::SizeT;

struct ArgError : std::exception {};

struct args_t {
    SizeT window_size;
};

args_t parse_arguments(int argc, char **argv);

constexpr string_view UsageHint = "Usage: ./ParserDriver <master-hostname <master-port>\n";

int main(int argc, char **argv) try {

} catch (const ArgError &e) {
}

args_t parse_arguments(int argc, char **argv) {
}
