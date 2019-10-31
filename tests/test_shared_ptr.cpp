//
//  main.cpp
//  SharedPtr
//
//  Created by Jaeyoon Kim on 9/10/19.
//  Copyright © 2019 Jaeyoon Kim. All rights reserved.
//

#include "../memory/shared_ptr.hpp"
#include <iostream>

using namespace std;
using namespace fb;

bool deleted = false;

class TestObj {
public:
    int publicInt = 0;
    ~TestObj() {
        deleted = true;
    }
};

int main(int argc, const char * argv[]) {
    WeakPtr<TestObj> wp;
    TestObj* t = new TestObj;
    {
        SharedPtr<TestObj> s1(t);
        wp = s1;
        assert(!wp.expired());
        {
            SharedPtr<TestObj> s2 = s1;
            assert(!wp.expired());
        }
        assert(!wp.expired());
        // s2 went out of scope but s1 still references t
        assert(!deleted);
        SharedPtr<TestObj> s3 = wp.lock();
        assert(s3->publicInt == 0);
        assert(!wp.expired());
    }
    // both s1 and s2 went out of scope
    assert(deleted);
    assert(wp.expired());
    
    SharedPtr<int> s4 = MakeShared<int>(0);
    assert(*s4 == 0);
    cout << "Success!\n";
    return 0;
}
