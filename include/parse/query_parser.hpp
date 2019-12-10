#pragma once

#include <fb/string.hpp>
#include <fb/string_view.hpp>

#include <parse/tokenstream.hpp>
#include <parse/expression.hpp>

/**
 * The actual expression parser
 */
class Parser {
public:
    // Stream of tokens to consume input from
    TokenStream stream;

    /**
     * Find the appropriate nonterminal
     *
     * Return nullptr if it could not be found
     */
    fb::UniquePtr<Expression> FindFactor() {
        if (stream.Match('(')) {
            auto orExpr = FindOr();
            if (orExpr && stream.Match(')'))
                return orExpr;
            else
                return nullptr;
        } else if (stream.Match('"')) {
            auto expr = fb::makeUnique<PhraseExpression>();
            while (true) {
                bool gotQuote = false;
                auto phrase = stream.ParseUntil([&gotQuote](char c) {
                    if (c == ' ')
                        return true;

                    if (c == '"') {
                        gotQuote = true;
                        return true;
                    }

                    return false;
                });

                if (phrase.empty())
                    return nullptr;

                expr->terms.pushBack(fb::makeUnique<WordExpression>(phrase));

                if (gotQuote)
                    break;
            }
            if (!stream.Match('"'))
                return nullptr;
            return expr;
        }

        auto word = stream.ParseWord();
        if (word.empty()) {
            return nullptr;
        } else
            return fb::makeUnique<WordExpression>(word);
    }

    fb::UniquePtr<Expression> FindOr() {
        auto left = FindSM();
        if (left) {
            auto orExpr = fb::makeUnique<OrExpression>();
            orExpr->terms.pushBack(std::move(left));
            while (stream.Match('|')) {
                left = FindSM();
                if (left)
                    orExpr->terms.pushBack(std::move(left));
                else
                    return nullptr;
            }
            return orExpr;
        }
        return nullptr;
    }

    fb::UniquePtr<Expression> FindAnd() {
        auto left = FindFactor();
        if (left) {
            auto andExpr = fb::makeUnique<AndExpression>();
            andExpr->terms.pushBack(std::move(left));
            while (stream.Match(' ')) {
                left = FindFactor();
                if (left)
                    andExpr->terms.pushBack(std::move(left));
                else
                    return nullptr;
            }
            return andExpr;
        }
        return nullptr;
    }

    fb::UniquePtr<Expression> FindSM() {
        auto left = FindAnd();
        if (left) {
            auto smExpr = fb::makeUnique<SMExpression>();
            smExpr->toReturn = std::move(left);
            if (stream.Match('\\')) {
                left = FindAnd();
                if (left) {
                    smExpr->notToReturn = std::move(left);
                    return smExpr;
                } else {
                    return nullptr;
                }
            } else {
                return smExpr;
            }
        }
        return nullptr;
    }


   /**
    * Construct parser based on given input
    */
    Parser( fb::String &in ) : stream(in) {}

   /**
    * The public interface of the parser. Call this function,
    * rather than the private internal functions.
    */
    fb::UniquePtr<Expression> Parse( ) {
        auto result = FindOr();
        if (!stream.AllConsumed())
            return nullptr;
        else
            return result;
    }
};
// class Parser
