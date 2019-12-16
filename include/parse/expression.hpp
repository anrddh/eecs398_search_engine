#pragma once

#include <isr/constraint_solver.hpp>
#include <isr/or_isr.hpp>
#include <isr/and_isr.hpp>
#include <isr/container_isr.hpp>
#include <isr/empty_isr.hpp>
#include <isr/phrase_isr.hpp>
#include <isr/document_isr.hpp>

#include <isr/index_reader.hpp>

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

    virtual fb::UniquePtr<ISR> Eval(IndexReader &) const = 0;
    virtual ConstraintSolver Constraints(IndexReader &reader) const {
        return {
            Eval(reader),
            reader.OpenDocumentISR(),
            Words(reader),
            static_cast<uint32_t>(reader.getIndex())
        };
    }
    virtual fb::Vector<fb::UniquePtr<WordISR>> Words(IndexReader &) const = 0;
    virtual void Print() const = 0;
};

/**
 * A word
 */
class WordExpression : public Expression {
public:
    fb::StringView word;

    WordExpression(fb::StringView wordIn) : word(wordIn) {}

    fb::UniquePtr<WordISR> WordEval(IndexReader &reader) const {
        fb::String word_str(word.data(), word.size());
        return reader.OpenWordISR(word_str);
    }

    virtual fb::UniquePtr<ISR> Eval(IndexReader &reader) const override {
        return WordEval(reader);
    }

    virtual fb::Vector<fb::UniquePtr<WordISR>> Words(IndexReader &reader) const override {
        fb::Vector<fb::UniquePtr<WordISR>> words;
        words.pushBack(WordEval(reader));
        return words;
    }

    virtual void Print() const override {
        std::cout << word;
    }
};

class AndExpression : public Expression {
public:
    fb::Vector<fb::UniquePtr<Expression>> terms;

    virtual fb::UniquePtr<ISR> Eval(IndexReader &reader) const override {
        fb::Vector<fb::UniquePtr<ISR>> ISRs;

        for (auto &term : terms)
            ISRs.pushBack(term->Eval(reader));

        return fb::makeUnique<AndISR>(std::move(ISRs), reader.OpenDocumentISR());
    }

    virtual void Print() const override {
        std::cout << '(' << ' ';
        if (terms.size())
            terms.front()->Print();

        for (size_t i = 1; i < terms.size(); ++i) {
            std::cout << "\t&\t";
            terms[i]->Print();
        }

        std::cout << ' ' << ')';
    }

    virtual fb::Vector<fb::UniquePtr<WordISR>> Words(IndexReader &reader) const override {
        fb::Vector<fb::UniquePtr<WordISR>> isrs;

        for (auto &term : terms)
            for (auto &wordIsr : term->Words(reader))
                isrs.pushBack(std::move(wordIsr));

        return isrs;
    }

};

class OrExpression : public Expression {
public:
    fb::Vector<fb::UniquePtr<Expression>> terms;

    virtual fb::UniquePtr<ISR> Eval(IndexReader &reader) const override {
        fb::Vector<fb::UniquePtr<ISR>> ISRs;

        for (auto &term : terms)
            ISRs.pushBack(term->Eval(reader));

        return fb::makeUnique<OrISR>(std::move(ISRs), reader.OpenDocumentISR());
    }

    virtual void Print() const override {
        std::cout << '(' << ' ';
        if (terms.size())
            terms.front()->Print();

        for (size_t i = 1; i < terms.size(); ++i) {
            std::cout << "\t|\t";
            terms[i]->Print();
        }

        std::cout << ' ' << ')';
    }

    virtual fb::Vector<fb::UniquePtr<WordISR>> Words(IndexReader &reader) const override {
        fb::Vector<fb::UniquePtr<WordISR>> isrs;

        for (auto &term : terms)
            for (auto &wordIsr : term->Words(reader))
                isrs.pushBack(std::move(wordIsr));

        return isrs;
    }
};

class PhraseExpression : public Expression {
public:
    fb::Vector<fb::UniquePtr<WordExpression>> terms;

    PhraseExpression() {}

    virtual fb::UniquePtr<ISR> Eval(IndexReader &reader) const override {
        fb::Vector<fb::UniquePtr<WordISR>> isrs;

        for (auto &term : terms)
            isrs.pushBack(term->WordEval(reader));

        return fb::makeUnique<PhraseISR>(std::move(isrs),
                                         reader.OpenDocumentISR());
    }

    virtual void Print() const override {
        std::cout << '"';
        if (terms.size())
            terms.front()->Print();

        for (size_t i = 1; i < terms.size(); ++i) {
            std::cout << " ";
            terms[i]->Print();
        }

        std::cout << '"';
    }

    virtual fb::Vector<fb::UniquePtr<WordISR>> Words(IndexReader &reader) const override {
        fb::Vector<fb::UniquePtr<WordISR>> isrs;

        for (auto &term : terms)
            for (auto &wordIsr : term->Words(reader))
                isrs.pushBack(std::move(wordIsr));

        return isrs;
    }
};

class SMExpression : public Expression {
public:
    fb::UniquePtr<Expression> toReturn;
    fb::UniquePtr<Expression> notToReturn;

    virtual fb::UniquePtr<ISR> Eval(IndexReader &reader) const override {
        return
            fb::makeUnique<ContainerISR>(toReturn->Eval(reader),
                         notToReturn ? notToReturn->Eval(reader) : fb::makeUnique<EmptyISR>(),
                         reader.OpenDocumentISR());
    }

    virtual void Print() const override {
        std::cout << '(' << ' ';
        toReturn->Print();
        std::cout << ' ' << ')';
        std::cout << " BUT NOT ";

        std::cout << '(' << ' ';
        if (notToReturn)
            notToReturn->Print();
        std::cout << ' ' << ')';
    }

    virtual fb::Vector<fb::UniquePtr<WordISR>> Words(IndexReader &reader) const override {
        return toReturn->Words(reader);
    }
};
