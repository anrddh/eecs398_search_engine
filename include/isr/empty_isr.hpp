#pragma once

#include <stdio.h> // perror

#include "isr.hpp"
#include "word_isr.hpp"


class EmptyISR : public WordISR {
public:
    EmptyISR() = default;
    ~EmptyISR() = default;

    virtual unsigned int GetDocumentCount( ) override;
    virtual unsigned int GetNumberOfOccurrences( ) override;
    virtual uint32_t GetDocumentId( ) override;
    virtual fb::UniquePtr<IndexInfo> GetCurrentInfo( ) override;
    virtual fb::UniquePtr<IndexInfo> Next( ) override;
    virtual fb::UniquePtr<IndexInfo> NextDocument( ) override;
    virtual fb::UniquePtr<IndexInfo> Seek( Location target ) override;
    virtual bool AtEnd( ) override;
    virtual fb::String GetWord( ) override;
};
