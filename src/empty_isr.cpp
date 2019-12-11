#include <isr/empty_isr.hpp>

unsigned int EmptyISR::GetDocumentCount() {
    return 0;
}

unsigned int EmptyISR::GetNumberOfOccurrences() {
    return 0;
}

uint32_t EmptyISR::GetDocumentId() {
    return 0;
}

fb::UniquePtr<IndexInfo> EmptyISR::GetCurrentInfo() {
    return nullptr;
}

fb::UniquePtr<IndexInfo> EmptyISR::Next() {
    return nullptr;
}

fb::UniquePtr<IndexInfo> EmptyISR::NextDocument() {
    return nullptr;
}

fb::UniquePtr<IndexInfo> EmptyISR::Seek(Location) {
    return nullptr;
}

bool EmptyISR::AtEnd() {
    return true;
}

fb::String EmptyISR::GetWord() {
    return "";
}
