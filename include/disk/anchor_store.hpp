#pragma once

#include <disk/disk_vec.hpp>

#include <fb/string.hpp>
#include <fb/string_view.hpp>
#include <fb/exception.hpp>
#include <fb/utility.hpp>
#include <fb/iterator.hpp>

#include <disk/logfile.hpp>
#include <debug.hpp>

#include <stdint.h>

//TEST
#include <iostream>

template <fb::SizeT TextSize_, typename T>
struct ChunkImpl {
    static constexpr fb::SizeT TextSize = TextSize_;

    char arr[TextSize];
    T ptr;
};

using Chunk = ChunkImpl<56, uint64_t>;

// Singleton
class AnchorStore {
public:
    static void init(fb::StringView filename) {
        delete ptr;
        ptr = new AnchorStore(filename);
        ptr->addStr("Dummy", {0,0});
    }

    static AnchorStore & getStore() {
        return *ptr;
    }

    // Recieves a new anchor text and a current begin and end offset
    // for the URL's anchor text. Adds the anchor text and returns the
    // new begin and end.  Note: the begin offset should only be
    // different if this is the first time seeing anchor text for this
    // url.
    fb::Pair<fb::SizeT, fb::SizeT> addStr(fb::StringView anchorText,
                                          fb::Pair<fb::SizeT,fb::SizeT> offsets) {
        // Special case: if the anchor text is empty we dont change
        // anything
        log(logfile,
            "Got anchor text: `", anchorText, "'",
            " with size ", anchorText.size(), '\n');

        if (anchorText.size() == 1)
            return offsets;

        auto &[beginOffset, endOffset] = offsets;

        if (!beginOffset) {
            beginOffset = anchors.reserve(1);
            auto numCharsCopied = anchorText.copy(anchors[beginOffset].arr,
                                                  Chunk::TextSize);

            if (numCharsCopied == Chunk::TextSize)
                endOffset = anchors[beginOffset].ptr = anchors.reserve(1);
            else
                endOffset = beginOffset;

            anchors[endOffset].arr[numCharsCopied % Chunk::TextSize] = 0;
            anchors[endOffset].ptr = 0;
            anchorText.removePrefix(numCharsCopied);
        }

        while (!anchorText.empty()) {
            auto nullTerm =
                fb::find(anchors[endOffset].arr,
                         anchors[endOffset].arr + Chunk::TextSize,
                         0);

            auto nullTermIdx = nullTerm - anchors[endOffset].arr;
            auto numToCopy = Chunk::TextSize - nullTermIdx;
            auto numCopied = anchorText.copy(nullTerm, numToCopy);
            if (numCopied == numToCopy)
                endOffset = anchors[endOffset].ptr = anchors.reserve(1);

            anchors[endOffset].arr[nullTermIdx + numCopied % numToCopy] = 0;
            anchors[endOffset].ptr = 0;
            anchorText.removePrefix(numCopied);
        }

        return offsets;
    }

    // Debug message
    void print(fb::Pair<fb::SizeT, fb::SizeT> offsets) {
        auto &[beginOffset, _] = offsets;
        if (!beginOffset)
           return;

        log(logfile, "Printing anchor text:", '\n');
        for (fb::SizeT i = beginOffset; i; i =  anchors[i].ptr)
            log(logfile,
                fb::StringView(anchors[i].arr,
                               fb::min(static_cast<long int>(Chunk::TextSize),
                                       fb::find(anchors[i].arr,
                                                anchors[i].arr + Chunk::TextSize,
                                                0) - anchors[i].arr)),
                '\n');
    }

    //Do we need this? not for now at least probably TODO
    //char *get_str(SizeT offset) {}
private:
    AnchorStore(fb::StringView filename) : anchors(filename) {
        addStr("", {0, 0});
    }

    static AnchorStore *ptr;
    DiskVec<Chunk> anchors;
};
