#pragma once

#include <fb/vector.hpp>
#include <fb/string_view.hpp>
#include <fb/memory.hpp>

#include <stddef.h>
#include <stdint.h>

/**
 * Just a plain old expression
 */
class Expression {
public:

    virtual ~Expression() = default;

    virtual void Eval() const = 0;
};

/**
 * A word
 */
class Word : public Expression {
public:
    fb::StringView word;

    Word(fb::StringView wordIn) : word(wordIn) {}

    void Eval( ) const override {
        std::cout << "Word Expression: " << word << '\n';
    }
};

class AndExpression : public Expression {
public:
    fb::Vector<fb::UniquePtr<Expression>> terms;

    void Eval() const override {
        /* Call And ISR */
        std::cout << "And Expression. Terms:\n";
        for (auto &term : terms)
            term->Eval();
        std::cout << '\n';
    }
};

class OrExpression : public Expression {
public:
    fb::Vector<fb::UniquePtr<Expression>> terms;

    void Eval( ) const override {
        /* Call Or ISR */
        std::cout << "Or Expression. Terms:\n";
        for (auto &term : terms)
            term->Eval();
        std::cout << '\n';
    }
};

class PhraseExpression : public Expression {
public:
    fb::Vector<fb::StringView> words;

    void Eval() const override {
        /* Call phrase ISR */
        std::cout << "Phrase Expression. Words:\n";
        for (auto word : words)
            std::cout << word << ',' << ' ';
        std::cout << '\n';
    }
};

class SMExpression : public Expression {
public:
    fb::UniquePtr<Expression> toReturn;
    fb::UniquePtr<Expression> notToReturn;

    void Eval( ) const override {
        std::cout << "SM Expression:\n";
        std::cout << "toReturn:\t"; toReturn->Eval();
        if (notToReturn) {
            std::cout << "notToReturn:\t";notToReturn->Eval();
        }
        /* Call IsR */
    }
};
