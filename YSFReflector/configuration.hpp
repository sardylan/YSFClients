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


#ifndef __YSF_REFLECTOR__CONFIGURATION__H
#define __YSF_REFLECTOR__CONFIGURATION__H

#include <string>
#include <vector>

enum SECTION {
    SECTION_NONE,
    SECTION_GENERAL,
    SECTION_INFO,
    SECTION_LOG,
    SECTION_NETWORK,
    SECTION_CLI
};


class Configuration {

public:

    Configuration();

    ~Configuration();

    bool read(const std::string &);

    bool isDaemon() const;

    const std::string &getName() const;

    const std::string &getDescription() const;

    unsigned int getLogDisplayLevel() const;

    unsigned int getLogFileLevel() const;

    const std::string &getLogFilePath() const;

    const std::string &getLogFileRoot() const;

    unsigned int getNetworkPort() const;

    bool isNetworkDebug() const;

    const std::string &getNetworkBlacklistFile() const;

    bool isCliEnabled() const;

    const std::string &getCliListenAddress() const;

    uint16_t getCliListenPort() const;

    const std::string &getCliUsername() const;

    const std::string &getCliPassword() const;

private:
    bool daemon;

    std::string name;
    std::string description;

    unsigned int logDisplayLevel;
    unsigned int logFileLevel;
    std::string logFilePath;
    std::string logFileRoot;

    unsigned int networkPort;
    bool networkDebug;
    std::string networkBlacklistFile;

    bool cliEnabled;
    std::string cliListenAddress;
    uint16_t cliListenPort;
    std::string cliUsername;
    std::string cliPassword;

};

#endif
