#include <string>

#include "doctest.h"
#include "mt.hpp"

TEST_CASE("starts_with_word") {
    CHECK(mt::starts_with_word("move", "move"));
    CHECK(mt::starts_with_word("move Pippo", "move"));
    CHECK(mt::starts_with_word("add player Pippo", "add player"));
    CHECK_FALSE(mt::starts_with_word("movePippo", "move"));
    CHECK_FALSE(mt::starts_with_word("add playerPippo", "add player"));
    CHECK_FALSE(mt::starts_with_word("mov", "move"));
    CHECK_FALSE(mt::starts_with_word("", "move"));
    CHECK_FALSE(mt::starts_with_word(" move", "move"));
}

TEST_CASE("trim") {
    CHECK(mt::trim_copy("  Pippo \t") == "Pippo");
    CHECK(mt::ltrim_copy("  Pippo ") == "Pippo ");
    CHECK(mt::rtrim_copy("  Pippo ") == "  Pippo");
    CHECK(mt::trim_copy("   ") == "");
}

TEST_CASE("string_format") {
    CHECK(mt::string_format("%s rolls %d", "Pippo", 4) == "Pippo rolls 4");
    CHECK(mt::string_format("%1$s and %1$s", "Pippo") == "Pippo and Pippo");
}
