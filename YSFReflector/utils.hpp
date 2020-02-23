/*
 *	Copyright (C) 2009,2014,2015 by Jonathan Naylor, G4KLX
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

#ifndef __YSF_REFLECTOR__UTILS__H
#define __YSF_REFLECTOR__UTILS__H

#include <string>

class Utils {

public:

    static void dump(const std::string &title, const unsigned char *data, unsigned int length);

    static void dump(int level, const std::string &title, const unsigned char *data, unsigned int length);

    static void dump(const std::string &title, const bool *bits, unsigned int length);

    static void dump(int level, const std::string &title, const bool *bits, unsigned int length);

    static void byteToBitsBE(unsigned char byte, bool *bits);

    static void byteToBitsLE(unsigned char byte, bool *bits);

    static void bitsToByteBE(const bool *bits, unsigned char &byte);

    static void bitsToByteLE(const bool *bits, unsigned char &byte);

    static std::string ltrim(const std::string &, const std::string &chars = "\t\n\v\f\r ");

    static std::string rtrim(const std::string &, const std::string &chars = "\t\n\v\f\r ");

    static std::string trim(const std::string &, const std::string &chars = "\t\n\v\f\r ");
};

#endif
