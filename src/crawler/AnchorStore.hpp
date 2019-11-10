#pragma once

#include "../../lib/string.hpp"
#include "../../lib/string_view.hpp"
#include "../../lib/exception.hpp"
#include "../../lib/utility.hpp"

#include "DiskVec.hpp"

#include <stdint.h>

template <fb::SizeT TextSize_, typename T>
struct ChunkImpl {
    static constexpr fb::SizeT TextSize = TextSize_;

    char arr[TextSize];
    T ptr;
};

using Chunk = ChunkImpl<56, uint64_t>;

class AnchorStore {
public:
    AnchorStore(fb::StringView filename) : anchors(filename) {
        addStr("", {0, 0});
    }

    //Recieves a new anchor text and a current begin and end offset for the
    //URL's anchor text. Adds the anchor text and returns the new begin and end.
    //Note: the begin offset should only be different if this is the first time
    //seeing anchor text for this url.
    fb::Pair<fb::SizeT, fb::SizeT>
    addStr(fb::StringView anchorText,
           fb::Pair<fb::SizeT, fb::SizeT> offsets) {
        // Special case: if the anchor text is empty we dont change
        // anything
        if (anchorText.empty())
            return offsets;

        auto &[beginOffset, endOffset] = offsets;

        // The loop
        while (!anchorText.empty()){
            auto idx = anchors.reserve(1);

            // beginOffset == 0 <=> url doese not have any anchor text
            // associated with it
            if (!beginOffset)
                offsets = { idx, idx };
            else
                endOffset = anchors[endOffset].ptr = idx;

            anchorText.copy(anchors.data() + idx, Chunk::TextSize);
            anchorText.remove_prefix(Chunk::TextSize);
        }

        anchors[endOffset].ptr = 0;
        return offsets;
    }

    //Do we need this? not for now at least probably TODO
    //char *get_str(SizeT offset) {}
private:
    DiskVec<Chunk> anchors;
};
