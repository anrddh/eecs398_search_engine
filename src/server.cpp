#include <bolt/bolt.hpp>

#include <fb/vector.hpp>
#include <fb/string.hpp>

fb::Vector<fb::String> carNames;

HtmlPage home( fb::UnorderedMap<fb::String, fb::String> formOptions ) {
  HtmlPage page;
  page.loadFromString("Hello World!");
  return page;
}

HtmlPage cars( fb::UnorderedMap<fb::String, fb::String> formOptions ) {
    HtmlPage page;
    page.loadFromString("<h1>Cars [% car %]!</h1><p>User Query=[% query %]</p>");

    fb::SizeT random = rand();
    page.setValue("car", carNames[random % carNames.size()]);
    page.setValue("query", formOptions["query"]);
    return page;
}

HtmlPage defaultPath() {
    HtmlPage page;
    page.loadFromString("<h1>Page is not found!</h1>");
    return page;
}

void setupCarNames() {
    carNames.pushBack("corvette");
    carNames.pushBack("mustang");
    carNames.pushBack("challenger");
    carNames.pushBack("camaro");
}

int main() {
    setupCarNames();

    Bolt bolt;
    bolt.registerHandler("/", home);
    bolt.registerHandler("/cars", cars);

   bolt.run();
}
