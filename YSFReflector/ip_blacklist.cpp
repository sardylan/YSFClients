/*
 *   Copyright (C) 2015,2016 by Jonathan Naylor G4KLX
 *                 2020 by Luca Cireddu IS0GVH
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <fstream>
#include <iostream>

#include "ip_blacklist.hpp"
#include "utils.hpp"

using namespace ysf;

IpBlacklist::IpBlacklist() {
    IpBlacklist::blacklistAddresses = new std::set<std::string>();
    IpBlacklist::mutex = new std::mutex();
}

IpBlacklist::~IpBlacklist() {
    delete mutex;
    delete blacklistAddresses;
}

bool IpBlacklist::addressAdd(const std::string &value) const {
    std::string address = Utils::trim(value);
    if (address.length() == 0)
        return false;

    mutex->lock();
    std::pair<std::set<std::string>::iterator, bool> result = blacklistAddresses->insert(address);
    mutex->unlock();

    return result.second;
}

bool IpBlacklist::addressRemove(const std::string &value) const {
    std::string address = Utils::trim(value);
    if (address.length() == 0)
        return false;

    mutex->lock();
    std::size_t erasedItems = blacklistAddresses->erase(address);
    mutex->unlock();

    return erasedItems == 1;
}

bool IpBlacklist::isAddressBlacklisted(const std::string &value) const {
    std::string address = Utils::trim(value);
    if (address.length() == 0)
        return false;

    bool res;

    mutex->lock();

    auto it = blacklistAddresses->find(address);
    res = it != blacklistAddresses->end();

    mutex->unlock();

    return res;
}

std::size_t IpBlacklist::countBlacklistedAddresses() const {
    std::size_t res;

    mutex->lock();
    res = blacklistAddresses->size();
    mutex->unlock();

    return res;
}

bool IpBlacklist::loadFromFile(const std::string &filePath) {
    std::ifstream fileStream;

    fileStream.open(filePath);
    if (!fileStream.good() || !fileStream.is_open())
        return false;

    std::string line;

    mutex->lock();

    while (std::getline(fileStream, line)) {
        std::string address = Utils::trim(line);
        if (address.length() > 0)
            blacklistAddresses->insert(line);
    }

    mutex->unlock();

    fileStream.close();

    return true;
}

bool IpBlacklist::saveToFile(const std::string &filePath) {
    std::ofstream fileStream;

    fileStream.open(filePath);
    if (!fileStream.good() || !fileStream.is_open())
        return false;

    mutex->lock();

    for (const std::string &address :*blacklistAddresses)
        fileStream << address << std::endl;

    mutex->unlock();

    fileStream.close();

    return true;
}
