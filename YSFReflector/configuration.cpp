/*
 *   Copyright (C) 2015,2016 by Jonathan Naylor G4KLX
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
 */

#include "configuration.hpp"
#include "log.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

#define BUFFER_SIZE  500

Configuration::Configuration() {
    Configuration::daemon = false;

    Configuration::name = "";
    Configuration::description = "";

    Configuration::logDisplayLevel = 0;
    Configuration::logFileLevel = 0;
    Configuration::logFilePath = "";
    Configuration::logFileRoot = "";

    Configuration::networkPort = 42000;
    Configuration::networkDebug = false;
    Configuration::networkBlacklistFile = "ipBlacklist.txt";

    Configuration::cliEnabled = true;
    Configuration::cliListenAddress = "0.0.0.0";
    Configuration::cliListenPort = 42023;
}

Configuration::~Configuration() = default;

bool Configuration::read(const std::string &m_file) {
    FILE *fp = ::fopen(m_file.c_str(), "rt");

    if (fp == nullptr) {
        ::fprintf(stderr, "Couldn't open the .ini file - %s\n", m_file.c_str());
        return false;
    }

    SECTION section = SECTION_NONE;

    char buffer[BUFFER_SIZE];
    while (::fgets(buffer, BUFFER_SIZE, fp) != nullptr) {
        if (buffer[0U] == '#')
            continue;

        if (buffer[0U] == '[') {
            if (::strncmp(buffer, "[General]", 9U) == 0)
                section = SECTION_GENERAL;
            else if (::strncmp(buffer, "[Info]", 6U) == 0)
                section = SECTION_INFO;
            else if (::strncmp(buffer, "[Log]", 5U) == 0)
                section = SECTION_LOG;
            else if (::strncmp(buffer, "[Network]", 9U) == 0)
                section = SECTION_NETWORK;
            else if (::strncmp(buffer, "[Cli]", 5U) == 0)
                section = SECTION_CLI;
            else
                section = SECTION_NONE;

            continue;
        }

        char *key = ::strtok(buffer, " \t=\r\n");
        if (key == nullptr)
            continue;

        char *value = ::strtok(nullptr, "\r\n");
        if (section == SECTION_GENERAL) {
            if (::strcmp(key, "Daemon") == 0)
                daemon = ::strtol(value, nullptr, 10) == 1;
        } else if (section == SECTION_INFO) {
            if (::strcmp(key, "Name") == 0)
                name = value;
            else if (::strcmp(key, "Description") == 0)
                description = value;
        } else if (section == SECTION_LOG) {
            if (::strcmp(key, "FilePath") == 0)
                logFilePath = value;
            else if (::strcmp(key, "FileRoot") == 0)
                logFileRoot = value;
            else if (::strcmp(key, "FileLevel") == 0)
                logFileLevel = (unsigned int) ::strtol(value, nullptr, 10);
            else if (::strcmp(key, "DisplayLevel") == 0)
                logDisplayLevel = (unsigned int) ::strtol(value, nullptr, 10);
        } else if (section == SECTION_NETWORK) {
            if (::strcmp(key, "Port") == 0)
                networkPort = (unsigned int) ::strtol(value, nullptr, 10);
            else if (::strcmp(key, "Debug") == 0)
                networkDebug = ::strtol(value, nullptr, 10) == 1;
            else if (::strcmp(key, "BlacklistFile") == 0)
                networkBlacklistFile = value;
        } else if (section == SECTION_CLI) {
            if (::strcmp(key, "Enabled") == 0)
                cliEnabled = ::strtol(value, nullptr, 10) == 1;
            else if (::strcmp(key, "ListenAddress") == 0)
                cliListenAddress = value;
            else if (::strcmp(key, "ListenPort") == 0)
                cliListenPort = ::strtol(value, nullptr, 10);
            else if (::strcmp(key, "Username") == 0)
                cliUsername = value;
            else if (::strcmp(key, "Password") == 0)
                cliPassword = value;
        }
    }

    ::fclose(fp);

    return true;
}

bool Configuration::isDaemon() const {
    return daemon;
}

const std::string &Configuration::getName() const {
    return name;
}

const std::string &Configuration::getDescription() const {
    return description;
}

unsigned int Configuration::getLogDisplayLevel() const {
    return logDisplayLevel;
}

unsigned int Configuration::getLogFileLevel() const {
    return logFileLevel;
}

const std::string &Configuration::getLogFilePath() const {
    return logFilePath;
}

const std::string &Configuration::getLogFileRoot() const {
    return logFileRoot;
}

unsigned int Configuration::getNetworkPort() const {
    return networkPort;
}

bool Configuration::isNetworkDebug() const {
    return networkDebug;
}

const std::string &Configuration::getNetworkBlacklistFile() const {
    return networkBlacklistFile;
}

bool Configuration::isCliEnabled() const {
    return cliEnabled;
}

const std::string &Configuration::getCliListenAddress() const {
    return cliListenAddress;
}

uint16_t Configuration::getCliListenPort() const {
    return cliListenPort;
}

const std::string &Configuration::getCliUsername() const {
    return cliUsername;
}

const std::string &Configuration::getCliPassword() const {
    return cliPassword;
}
