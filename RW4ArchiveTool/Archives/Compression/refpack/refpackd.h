#pragma once

#include <algorithm>
#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdint>
#include <unordered_map>

namespace refpack
{
    class ByteAccess {
    private:
        bool bigendian;
        std::string efmt;

    public:
        ByteAccess(bool bigendian = true) {
            this->bigendian = bigendian;
            setEndianFormat();
        }

        void setEndianFormat() {
            if (bigendian) {
                efmt = ">";
            }
            else {
                efmt = "<";
            }
        }

        uint32_t getUInt32(const std::string& b, size_t offset = 0) {
            uint32_t value;
            std::memcpy(&value, b.data() + offset, sizeof(uint32_t));
            return value;
        }

        float getFloat(const std::string& b, size_t offset = 0) {
            float value;
            std::memcpy(&value, b.data() + offset, sizeof(float));
            return value;
        }

        uint16_t getUInt16(const std::string& b, size_t offset = 0) {
            uint16_t value;
            std::memcpy(&value, b.data() + offset, sizeof(uint16_t));
            return value;
        }

        uint8_t getUInt8(const std::string& b, size_t offset = 0) {
            uint8_t value;
            std::memcpy(&value, b.data() + offset, sizeof(uint8_t));
            return value;
        }

        std::string putUInt32(uint32_t v) {
            std::string buffer(sizeof(uint32_t), '\0');
            std::memcpy(&buffer[0], &v, sizeof(uint32_t));
            return buffer;
        }

        std::string putUInt16(uint16_t v) {
            std::string buffer(sizeof(uint16_t), '\0');
            buffer[0] = static_cast<uint8_t>(v >> 8);
            buffer[1] = static_cast<uint8_t>(v);
            return buffer;
        }

        std::string putFloat(float v) {
            std::string buffer(sizeof(float), '\0');
            std::memcpy(&buffer[0], &v, sizeof(float));
            return buffer;
        }

        std::string putFloatArray(const std::vector<float>& v) {
            std::string buffer(sizeof(float) * v.size(), '\0');
            std::memcpy(&buffer[0], v.data(), sizeof(float) * v.size());
            return buffer;
        }

        std::string putByteArray(const std::vector<uint8_t>& v) {
            std::string buffer(v.size(), '\0');
            std::memcpy(&buffer[0], v.data(), v.size());
            return buffer;
        }

        std::string putUInt8(uint8_t v) {
            std::string buffer(sizeof(uint8_t), '\0');
            std::memcpy(&buffer[0], &v, sizeof(uint8_t));
            return buffer;
        }
    };

    //-------------------------------------------------------------------
    //   Refpack
    //-------------------------------------------------------------------


    // Refpack file signature
    constexpr uint16_t REF_SIG = 0x10fb;  // ushort
    constexpr uint16_t REF_SIG_MASK = 0x3eff;  // ushort

    // Long references were used when compressing this data;
    // a 128K back reference buffer is required for streamed
    // decompression.
    constexpr uint16_t REF_HEADER_FLAG_SHORT_REFS = 0x4000;  // ushort

    // input file size is larger than 2^24; sizeof(size) field is 32 bits.
    constexpr uint16_t REF_HEADER_FLAG_SIZE32 = 0x8000;  // ushort

    constexpr int MAX_LOOKBACK_SHORT = (16 * 1024) - 1;
    constexpr int MAX_LOOKBACK_LONG = (128 * 1024) - 1;

    constexpr int HASHTABLESIZE = 16 * 1024;  // 64 * 1024

    //  table for mapping levels to windowsize/quick factors.
    std::unordered_map<int, std::pair<int, int>> QMapTable{
        {0, {0, 0}},
        {1, {3, 1 * 1024 - 1}},
        {2, {3, 16 * 1024 - 1}},
        {3, {1, 1 * 1024 - 1}},
        {4, {1, 2 * 1024 - 1}},
        {5, {1, 4 * 1024 - 1}},
        {6, {0, 16 * 1024 - 1}},
        {7, {0, 32 * 1024 - 1}},
        {8, {0, 64 * 1024 - 1}},
        {9, {0, 128 * 1024 - 1}}
    };


    // ------------------------------------------------------------------------
    int32_t _match_length(const uint8_t* src, int32_t sindex, int32_t dindex, int32_t maxmatch) {
        int32_t current = 0;

        while (maxmatch >= 4) {
            if (src[sindex + 0] != src[dindex + 0]) return current + 0;
            if (src[sindex + 1] != src[dindex + 1]) return current + 1;
            if (src[sindex + 2] != src[dindex + 2]) return current + 2;
            if (src[sindex + 3] != src[dindex + 3]) return current + 3;

            sindex += 4;
            dindex += 4;
            current += 4;
            maxmatch -= 4;
        }

        if (maxmatch < 1 || src[sindex + 0] != src[dindex + 0]) return current + 0;
        if (maxmatch < 2 || src[sindex + 1] != src[dindex + 1]) return current + 1;
        if (maxmatch < 3 || src[sindex + 2] != src[dindex + 2]) return current + 2;
        if (maxmatch < 4 || src[sindex + 3] != src[dindex + 3]) return current + 3;

        return current + 4;
    }


    // ------------------------------------------------------------------------
    int32_t _get_hash(const uint8_t* src, int32_t offset) {
        int32_t hsh = src[offset + 0] * 1089 + src[offset + 1] * 33 + src[offset + 2];
        return hsh & (HASHTABLESIZE - 1);
    }


    // ------------------------------------------------------------------------
    // Refpack compress.
    // level is a number from 1 to 9
    //   1 - Minimal Compression Ratio (best speed, fewest resources).
    //   9 - Maximum compression ratio, slowest speed
    std::string compress(const std::vector<uint8_t>& data, int level, bool littleendian = false) {
        level = min(max(level, 1), 9);

        ByteAccess ba;

        std::vector<uint8_t> src;
        if (data.size() > 0) {
            src = data;
        }

        std::string output;

        int tlen = 0;
        int tcost = 0;
        int run = 0;
        int toffset = 0;
        int boffset = 0;
        int blen = 0;
        int bcost = 0;
        int mlen = 0;

        // position pointers
        int tptr = 0;
        int cptr = 0;
        int rptr = 0;

        // hash
        int hash_ = 0;
        int hoffset = 0;
        int minhoffset = 0;
        std::vector<uint8_t> tmp;
        int refsig = REF_SIG;

        int sizebytes = 3;

        // clamp the level between 1-9
        level = min(max(level, 1), 9);

        int quick, max_lookback;
        std::tie(quick, max_lookback) = QMapTable[level];

        std::vector<int> hashtbl(HASHTABLESIZE, 0);
        std::vector<int> link(max_lookback + 1, 0);

        int src_length = src.size();

        // if source size >= 2^24  Use 32-bit header
        if (src_length >= 0x1000000) {
            refsig |= REF_HEADER_FLAG_SIZE32;
            sizebytes = 4;
        }

        // write the signature
        output += ba.putUInt16(refsig);

        if (sizebytes == 3) {
            std::vector<uint8_t> cast = {
                static_cast<uint8_t>((src_length >> 24) & 0xFF),
                static_cast<uint8_t>((src_length >> 16) & 0xFF),
                static_cast<uint8_t>((src_length >> 8) & 0xFF),
                static_cast<uint8_t>(src_length & 0xFF)
            };

            // We assume that the MSB will be discarded in both cases
            if (littleendian) {
                output += ba.putByteArray({ cast[2], cast[1], cast[0] });
            }
            else {
                output += ba.putByteArray({ cast[1], cast[2], cast[3] });
            }
        }
        else {
            ba.putUInt32(src_length);
        }

        int src_buffer_size = src_length - 4;

        while (src_buffer_size >= 0) {
            boffset = 0;
            blen = 2;
            bcost = 2;
            mlen = min(src_buffer_size, 1028);
            tptr = cptr - 1;
            hash_ = _get_hash(src.data(), cptr);
            hoffset = hashtbl[hash_];
            minhoffset = max(cptr - max_lookback, 1);

            if (hoffset >= minhoffset) {
                while (true) {
                    tptr = hoffset;
                    if (src[cptr + blen] == src[tptr + blen]) {
                        tlen = _match_length(src.data(), cptr, tptr, mlen);
                        toffset = (cptr - 1) - tptr;

                        if (toffset < 1024 && tlen <= 10) {
                            tcost = 2;  // two-byte short form
                        }
                        else if (toffset < 16384 && tlen <= 67) {
                            tcost = 3;  // three-byte long form
                        }
                        else {
                            tcost = 4;  // four-byte very long form
                        }

                        if (tlen - tcost + 4 > blen - bcost + 4) {
                            blen = tlen;
                            bcost = tcost;
                            boffset = toffset;
                            if (blen >= 1028) {
                                break;
                            }
                        }
                    }

                    hoffset = link[hoffset & max_lookback];
                    if (!(hoffset >= minhoffset)) {
                        break;
                    }
                }
            }

            if (bcost >= blen || src_buffer_size < 4) {
                hoffset = cptr;
                link[hoffset & max_lookback] = hashtbl[hash_];
                hashtbl[hash_] = hoffset;
                run += 1;
                cptr += 1;
                src_buffer_size -= 1;
            }
            else {
                while (run > 3) {  // literal block of data
                    tlen = min(112, run & ~3);
                    run -= tlen;
                    output += ba.putUInt8((0xe0 + (tlen >> 2) - 1) & 0xFF);
                    tmp = std::vector<uint8_t>(src.begin() + rptr, src.begin() + rptr + tlen);
                    output += ba.putByteArray(tmp);
                    rptr += tlen;
                }

                if (bcost == 2) {  // two-byte long form
                    output += ba.putUInt8((((boffset >> 8) << 5) + ((blen - 3) << 2) + run) & 0xFF);
                    output += ba.putUInt8(boffset & 0xFF);
                }
                else if (bcost == 3) {  // three-byte long form
                    output += ba.putUInt8((0x80 + (blen - 4)) & 0xFF);
                    output += ba.putUInt8(((run << 6) + (boffset >> 8)) & 0xFF);
                    output += ba.putUInt8(boffset & 0xFF);
                }
                else {  // four-byte very long form
                    output += ba.putUInt8((0xc0 + ((boffset >> 16) << 4) + (((blen - 5) >> 8) << 2) + run) & 0xFF);
                    output += ba.putUInt8((boffset >> 8) & 0xFF);
                    output += ba.putUInt8(boffset & 0xFF);
                    output += ba.putUInt8((blen - 5) & 0xFF);
                }

                if (run > 0) {
                    tmp = std::vector<uint8_t>(src.begin() + rptr, src.begin() + rptr + run);
                    output += ba.putByteArray(tmp);
                    run = 0;
                }

                if (quick > 0) {
                    if (quick == 3) {
                        hoffset = cptr;
                        link[hoffset & max_lookback] = 0;
                        hashtbl[hash_] = hoffset;
                    }
                    else if (quick == 1 || (hoffset & 3) == 0) {
                        hoffset = cptr;
                        link[hoffset & max_lookback] = hashtbl[hash_];
                        hashtbl[hash_] = hoffset;
                    }

                    cptr += blen;
                }
                else {
                    for (int i = 0; i < blen; i++) {
                        hash_ = _get_hash(src.data(), cptr);
                        hoffset = cptr;
                        link[hoffset & max_lookback] = hashtbl[hash_];
                        hashtbl[hash_] = hoffset;
                        cptr += 1;
                    }
                }

                rptr = cptr;
                src_buffer_size -= blen;
            }
        }

        src_buffer_size += 4;
        run += src_buffer_size;

        // No match at the end, use literal
        while (run > 3) {
            tlen = min(112, run & ~3);
            run -= tlen;

            output += ba.putUInt8((0xe0 + (tlen >> 2) - 1) & 0xFF);

            tmp = std::vector<uint8_t>(src.begin() + rptr, src.begin() + rptr + tlen);
            output += ba.putByteArray(tmp);
            rptr += tlen;
        }

        // End of stream command + 0..3 literal
        output += ba.putUInt8((0xfc + run) & 0xFF);

        if (run > 0) {
            tmp = std::vector<uint8_t>(src.begin() + rptr, src.begin() + rptr + run);
            output += ba.putByteArray(tmp);
        }

        return output;
    }

    // ------------------------------------------------------------------------
    // Copy bytes from one array to another.
    size_t _copyn(const std::vector<uint8_t>& input, size_t sindex, std::vector<uint8_t>& output, size_t dindex, size_t run) {
        while (run > 0) {
            output[dindex] = input[sindex];
            dindex++;
            sindex++;
            run--;
        }

        return dindex;
    }

    // ------------------------------------------------------------------------
    // Return the decompressed size of a Refpack-compressed buffer.
    std::tuple<size_t, size_t> get_decompressed_size(const std::vector<uint8_t>& src) {
        size_t size = 0;
        size_t dataoffset = 0;

        if (src[0] == 0x10 && src[1] == 0xfb) { // 3 byte size field
            size = (src[2] << 16) | (src[3] << 8) | src[4];
            dataoffset = 5;
        }
        else if (src[0] == 0x90 && src[1] == 0xfb) { // 4 byte size field
            size = (src[2] << 24) | (src[3] << 16) | (src[4] << 8) | src[5];
            dataoffset = 6;
        }

        return std::make_tuple(size, dataoffset);
    }


    // ------------------------------------------------------------------------
    // Decompress a Refpack-compressed buffer.
    std::vector<uint8_t> decompress(const std::vector<uint8_t>& data) {
        std::vector<uint8_t> output;
        bool as_string = false;
        std::vector<uint8_t> src;

        src = data;

        size_t size, dataoffset;
        std::tie(size, dataoffset) = get_decompressed_size(src);

        output.resize(size);

        uint8_t first = 0x00;
        uint8_t second = 0x00;
        uint8_t third = 0x00;
        uint8_t fourth = 0x00;
        size_t s = dataoffset;
        size_t d = 0;

        while (true) {
            size_t r = 0;
            size_t run = 0;
            first = src[s];

            if ((first & 0x80) == 0) {
                // short form
                second = src[s + 1];
                s += 2;
                run = first & 3;

                if (run > 0) {
                    d = _copyn(src, s, output, d, run);
                    s += run;
                }

                r = d - 1 - (((first & 0x60) << 3) + second);
                run = ((first & 0x1c) >> 2) + 3;

                d = _copyn(output, r, output, d, run);
                continue;
            }

            if ((first & 0x40) == 0) {
                // long form
                second = src[s + 1];
                third = src[s + 2];
                s += 3;
                run = second >> 6;

                if (run > 0) {
                    d = _copyn(src, s, output, d, run);
                    s += run;
                }

                r = d - 1 - (((second & 0x3f) << 8) + third);
                run = (first & 0x3f) + 4;
                d = _copyn(output, r, output, d, run);
                continue;
            }

            if ((first & 0x20) == 0) {
                // very long form
                second = src[s + 1];
                third = src[s + 2];
                fourth = src[s + 3];
                s += 4;
                run = first & 3;

                if (run > 0) {
                    d = _copyn(src, s, output, d, run);
                    s += run;
                }

                r = d - 1 - (((first & 0x10) >> 4 << 16) + (second << 8) + third);
                run = ((first & 0x0c) >> 2 << 8) + fourth + 5;
                d = _copyn(output, r, output, d, run);
                continue;
            }

            s += 1;

            run = ((first & 0x1f) << 2) + 4;  // literal

            if (run <= 112) {
                d = _copyn(src, s, output, d, run);
                s += run;
                continue;
            }

            run = first & 3;  // eof (+0..3 literal)

            if (run > 0) {
                d = _copyn(src, s, output, d, run);
                s += run;
            }

            break;
        }

        return output;
    }
}
