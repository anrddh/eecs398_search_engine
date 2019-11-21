#include <disk/page_store.hpp>

using namespace fb;

using Page = fb::Pair<fb::SizeT, fb::Pair<fb::StringView, fb::Vector<WordDescriptors>>>;

int main(){

    initializeFileName("/tmp/PageStore");

    Vector<WordDescriptors> v1; v1.pushBack(3); v1.pushBack(5); v1.pushBack(7); v1.pushBack(11);
    Page p1 = { 2, {"string1 for the first page", v1} };
    Page p2 = { 4, {"string2 for the second page", v1} };
    Page p3 = { 6, {"string3 for the third page", v1} };
    Page p4 = { 8, {"string4 for the fourth page", v1} };
    Page p5 = { 9, {"string5 for the fifth page", v1} };
    Page p6 = p5; Page p7 = p4; Page p8 = p5; Page p9 = p2;

    addPage(p1); addPage(p2); addPage(p3); addPage(p4); addPage(p5);
    addPage(p6); addPage(p7); addPage(p8); addPage(p9); addPage(p9);

    for (int i = 0; i < 200; ++i) addPage(p1);


    while(NumThreads);
}
