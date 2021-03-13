#pragma once

#include "decompressioncontext.h"
#include "util/util.h"

namespace pboman3 {
    class Lzh {
    public:
        static void decompress(QFileDevice* source, QFileDevice* target, int outputLength, const Cancel& cancel);

        static void compress(QFileDevice* source, QFileDevice* target, const Cancel& cancel);

    private:
        static void processBlock(DecompressionContext& ctx);

        static bool isValid(DecompressionContext& ctx);

        static void writeCrc(QFileDevice* source, QFileDevice* target);

    };
}
