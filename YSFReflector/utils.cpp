/*
 *	Copyright (C) 2009,2014,2015,2016 Jonathan Naylor, G4KLX
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; version 2 of the License.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 */

#include "utils.hpp"
#include "Log.h"

#include <cstdio>
#include <cassert>

void Utils::dump(const std::string &title, const unsigned char *data, unsigned int length) {
    assert(data != nullptr);

    dump(2U, title, data, length);
}

void Utils::dump(int level, const std::string &title, const unsigned char *data, unsigned int length) {
    assert(data != nullptr);

    ::Log(level, "%s", title.c_str());

    unsigned int offset = 0U;

    while (length > 0U) {
        std::string output;

        unsigned int bytes = (length > 16U) ? 16U : length;

        for (unsigned i = 0U; i < bytes; i++) {
            char temp[10U];
            ::sprintf(temp, "%02X ", data[offset + i]);
            output += temp;
        }

        for (unsigned int i = bytes; i < 16U; i++)
            output += "   ";

        output += "   *";

        for (unsigned i = 0U; i < bytes; i++) {
            unsigned char c = data[offset + i];

            if (::isprint(c))
                output += c;
            else
                output += '.';
        }

        output += '*';

        ::Log(level, "%04X:  %s", offset, output.c_str());

        offset += 16U;

        if (length >= 16U)
            length -= 16U;
        else
            length = 0U;
    }
}

void Utils::dump(const std::string &title, const bool *bits, unsigned int length) {
    assert(bits != nullptr);

    dump(2U, title, bits, length);
}

void Utils::dump(int level, const std::string &title, const bool *bits, unsigned int length) {
    assert(bits != nullptr);

    unsigned char bytes[100U];
    unsigned int nBytes = 0U;
    for (unsigned int n = 0U; n < length; n += 8U, nBytes++)
        bitsToByteBE(bits + n, bytes[nBytes]);

    dump(level, title, bytes, nBytes);
}

void Utils::byteToBitsBE(unsigned char byte, bool *bits) {
    assert(bits != nullptr);

    bits[0U] = (byte & 0x80U) == 0x80U;
    bits[1U] = (byte & 0x40U) == 0x40U;
    bits[2U] = (byte & 0x20U) == 0x20U;
    bits[3U] = (byte & 0x10U) == 0x10U;
    bits[4U] = (byte & 0x08U) == 0x08U;
    bits[5U] = (byte & 0x04U) == 0x04U;
    bits[6U] = (byte & 0x02U) == 0x02U;
    bits[7U] = (byte & 0x01U) == 0x01U;
}

void Utils::byteToBitsLE(unsigned char byte, bool *bits) {
    assert(bits != nullptr);

    bits[0U] = (byte & 0x01U) == 0x01U;
    bits[1U] = (byte & 0x02U) == 0x02U;
    bits[2U] = (byte & 0x04U) == 0x04U;
    bits[3U] = (byte & 0x08U) == 0x08U;
    bits[4U] = (byte & 0x10U) == 0x10U;
    bits[5U] = (byte & 0x20U) == 0x20U;
    bits[6U] = (byte & 0x40U) == 0x40U;
    bits[7U] = (byte & 0x80U) == 0x80U;
}

void Utils::bitsToByteBE(const bool *bits, unsigned char &byte) {
    assert(bits != nullptr);

    byte = bits[0U] ? 0x80U : 0x00U;
    byte |= bits[1U] ? 0x40U : 0x00U;
    byte |= bits[2U] ? 0x20U : 0x00U;
    byte |= bits[3U] ? 0x10U : 0x00U;
    byte |= bits[4U] ? 0x08U : 0x00U;
    byte |= bits[5U] ? 0x04U : 0x00U;
    byte |= bits[6U] ? 0x02U : 0x00U;
    byte |= bits[7U] ? 0x01U : 0x00U;
}

void Utils::bitsToByteLE(const bool *bits, unsigned char &byte) {
    assert(bits != nullptr);

    byte = bits[0U] ? 0x01U : 0x00U;
    byte |= bits[1U] ? 0x02U : 0x00U;
    byte |= bits[2U] ? 0x04U : 0x00U;
    byte |= bits[3U] ? 0x08U : 0x00U;
    byte |= bits[4U] ? 0x10U : 0x00U;
    byte |= bits[5U] ? 0x20U : 0x00U;
    byte |= bits[6U] ? 0x40U : 0x00U;
    byte |= bits[7U] ? 0x80U : 0x00U;
}

std::string Utils::ltrim(const std::string &str, const std::string &chars) {
    std::string value = str;
    value.erase(0, str.find_first_not_of(chars));
    return value;
}

std::string Utils::rtrim(const std::string &str, const std::string &chars) {
    std::string value = str;
    value.erase(str.find_last_not_of(chars) + 1);
    return value;
}

std::string Utils::trim(const std::string &str, const std::string &chars) {
    return ltrim(rtrim(str, chars), chars);
}