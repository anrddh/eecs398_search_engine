#include <bolt/bolt.hpp>

#include <fb/vector.hpp>
#include <fb/string.hpp>

HtmlPage home( fb::UnorderedMap<fb::String, fb::String> formOptions ) {
    HtmlPage page;
    page.loadFromFile("frontend/title.html");
    return page;
}

int resultCounter = -1;
fb::UnorderedMap<fb::String, fb::String> resultOptions;

HtmlPage kthResults() {
    HtmlPage page;
    page.loadFromFile("frontend/search_results.html");

    /* Invoke ranker */

    fb::SizeT random = rand();
    page.setValue("query", resultOptions["query"]);

    fb::String counterString = fb::toString(resultCounter);

    if( resultOptions["query"] == "fantastic+bugs" )
    {
        page.setValue("title0", "Fanatstic Bugs Presentation");
        page.setValue("url0", "https://docs.google.com/presentation/d/1VGzVglZy6j6fE8RD95SxyeLF8HCY3NdpRwY29NMRGEw/edit#slide=id.p");
        page.setValue("snippet0", "");
    }

    else if( resultOptions["query"] == "aniruddh" )
    {
        page.setValue("title0", "Aniruddh Agarwal - MATH 593 (Graduate Algebra I) Grader...");
        page.setValue("url0", "https://www.linkedin.com/in/agarwalaniruddh");
        page.setValue("snippet0", "Undergraduate student at the University of Michigan majoring in Computer Science and Mathematics. Interested in systems programming and machine learning.");

        page.setValue("title1", "User Aniruddh Agarwal - MathOverflow");
        page.setValue("url1", "https://mathoverflow.net/users/136734/aniruddh-agarwal");
        page.setValue("snippet1", "Undergrad at Michigan interested in arithmetic geometry.");

        page.setValue("title2", "Aniruddh Agarwal");
        page.setValue("url2", "https://www.facebook.com/aniruddh.agarwal.3");
        page.setValue("snippet2", "");

        page.setValue("title3", "Aniruddh Agarwal");
        page.setValue("url3", "https://github.com/anrddh");
        page.setValue("snippet3", "Aniruddh Agarwal anrddh");

        page.setValue("title4", "User Aniruddh Agarwal - Stack Overflow");
        page.setValue("url4", "https://stackoverflow.com/users/11428421/aniruddh-agarwal");
        page.setValue("snippet4", "Undergrad at Michigan interested in arithmetic geometry.");
    }
    else
    {
        // int numResults = 10;
        // for(int i = 0; i < numResults; ++i) {
        //     fb::String iString = fb::toString(i);
        //     page.setValue("title" + iString, counterString +  iString + "th Result for " + resultOptions["query"]);
        //     page.setValue("url" + iString, counterString + iString + "th Url");
        //     page.setValue("snippet" + iString, counterString + iString + "th Snippet");
        // }
        page.setValue("title0", "Did you mean Nicole Hamilton?");
        page.setValue("url0" , "https://hamiltonlabs.com/");

        page.setValue("title1", "Hamilton C shell");
        page.setValue("url1", "https://en.wikipedia.org/wiki/Hamilton_C_shell");
        page.setValue("snippet1", "Hamilton C shell is a clone of the Unix C shell and utilities[1][2] for Microsoft Windows created by Nicole Hamilton[3] at Hamilton Laboratories as a completely original work, not based on any prior code. It was first released on OS/2 on December 12, 1988[4][5][6][7][8][9] and on Windows NT in July 1992.[10][11][12] The OS/2 version was discontinued in 2003 but the Windows version continues to be actively supported.");

        page.setValue("title2", "EECS 280: Programming and Intro Data Structures");
        page.setValue("url2", "https://eecs280staff.github.io/eecs280.org/");
        page.setValue("snippet2", "Computer science fundamentals, with programming in C++. Build an image processing program, a game of Euchre, a web backend, and a machine learning algorithm.");
    }

    return page;
}

HtmlPage results( fb::UnorderedMap<fb::String, fb::String> formOptions ) {
    resultCounter = 0;
    resultOptions = formOptions;
    return kthResults();
}

HtmlPage defaultPath() {
    HtmlPage page;
    page.loadFromString("<h1>Page not found.</h1>");
    return page;
}

HtmlPage next( fb::UnorderedMap<fb::String, fb::String> formOptions ) {
    if( resultCounter == -1 )
        return defaultPath();
    ++resultCounter;
    return kthResults();
}

HtmlPage previous( fb::UnorderedMap<fb::String, fb::String> formOptions ) {
    if( resultCounter <= 0 )
        return defaultPath();
    --resultCounter;
    return kthResults();
}

int main() {
    Bolt bolt;
    bolt.registerHandler("/", home);
    bolt.registerHandler("/results", results);
    bolt.registerHandler("/next", next);
    bolt.registerHandler("/previous", previous);

    bolt.run();
}
