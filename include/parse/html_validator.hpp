//#pragma once

//FAR FROM DONE, DO NOT USE

#include <string>
#include <stack>
#include <fstream>
#include <regex>

namespace fb {

typedef std::string html_tag;

class html_validator {
public:
    // Flags for the validator to use to determine how strict it will be
    bool enforce_doctype = true;
    bool enforce_closure_on_p = true;
    bool enforce_close_empty = false;
    bool enforce_head = false;
    bool enforce_body = false;
    
    //Default constructor
    html_validator() {};

    //Constructor with validation flags
    html_validator(bool e_d, bool e_c_o_p, bool e_c_e, bool e_h, bool e_b) :
        enforce_doctype(e_d), enforce_closure_on_p(e_c_o_p), enforce_close_empty(e_c_e),
        enforce_head(e_h), enforce_body(e_b) {};

    //Resets the validator (empties the stack), should be called between each validation
    void reset() {
        while (!stack.empty()) stack.pop();
    }

    //Exception type for popping empty stack
    class empty_stack { };

private:
    std::stack<html_tag> stack;
    std::regex tag_format("</?\s?(\w|!|-)+\s?.*/?>"); //NOTE: TEST THIS REGEX!!!

    //TODO: uses stack method to validate file according to set flags.
    //Returns file offset where validation fails, or -1 if validation succeeds
    int validate_file(std::istream infile) {
        std::string current_line; 
        while (current_line.getline(infile)) {
            std::match_results results = std::regex_search(current_line, tag_format);
        }
    };

};



} //fb