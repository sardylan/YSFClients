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


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctime>
#include <sys/types.h>
#include <unistd.h>
#include <csignal>
#include <fcntl.h>
#include <pwd.h>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <ctime>
#include <cstring>
#include <iostream>

#include "ysf_reflector.hpp"
#include "ysf_repeater.hpp"
#include "ip_blacklist.hpp"
#include "StopWatch.h"
#include "Network.h"
#include "Version.h"
#include "Thread.h"
#include "log.hpp"

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

const char *DEFAULT_INI_FILE = "YSFReflector.ini";

using namespace ysf;


int main(int argc, char **argv) {
    const char *iniFile = DEFAULT_INI_FILE;

    if (argc > 1) {
        for (int currentArg = 1; currentArg < argc; ++currentArg) {
            std::string arg = argv[currentArg];
            if ((arg == "-v") || (arg == "--version")) {
                ::fprintf(stdout, "YSFReflector version %s\n", VERSION);
                return 0;
            } else if (arg.substr(0, 1) == "-") {
                ::fprintf(stderr, "Usage: YSFReflector [-v|--version] [filename]\n");
                return 1;
            } else {
                iniFile = argv[currentArg];
            }
        }
    }

    signal(SIGINT, signalHandler);

    auto *ysfReflector = YSFReflector::getInstance();

    ysfReflector->run(iniFile);

    return 0;
}

void signalHandler(int signum) {
    if (signum == SIGINT) {
        auto *ysfReflector = YSFReflector::getInstance();
        ysfReflector->stop();
    }
}

YSFReflector *YSFReflector::instance = nullptr;

YSFReflector::YSFReflector() {
    keepRunning = true;

    configuration = new Configuration();
    ipBlacklist = new IpBlacklist();
    cliServer = new CliServer(this);
}

YSFReflector::~YSFReflector() {
    delete cliServer;
    delete ipBlacklist;
    delete configuration;
}

YSFReflector *YSFReflector::getInstance() {
    if (YSFReflector::instance == nullptr)
        YSFReflector::instance = new YSFReflector();

    return YSFReflector::instance;
}

void YSFReflector::run(const std::string &iniFile) {
    bool ret = configuration->read(iniFile);
    if (!ret) {
        ::fprintf(stderr, "YSFReflector: cannot read the .ini file\n");
        return;
    }

    ret = ::LogInitialise(
            configuration->getLogFilePath(),
            configuration->getLogFileRoot(),
            configuration->getLogFileLevel(),
            configuration->getLogDisplayLevel()
    );
    if (!ret) {
        ::fprintf(stderr, "YSFReflector: unable to open the log file\n");
        return;
    }

    CNetwork network(
            configuration->getNetworkPort(),
            configuration->getName(),
            configuration->getDescription(),
            configuration->isNetworkDebug()
    );

    ret = network.open();
    if (!ret) {
        ::LogFinalise();
        return;
    }

    ipBlacklist->loadFromFile(configuration->getNetworkBlacklistFile());

    cliServer->setListenAddress(configuration->getCliListenAddress());
    cliServer->setListenPort(configuration->getCliListenPort());
    cliServer->setUsername(configuration->getCliUsername());
    cliServer->setPassword(configuration->getCliPassword());

    if (configuration->isCliEnabled())
        cliServer->start();

    network.setCount(0);

    CStopWatch stopWatch;
    stopWatch.start();

    CTimer dumpTimer(1000U, 120U);
    dumpTimer.start();

    CTimer pollTimer(1000U, 5U);
    pollTimer.start();

    LogMessage("Starting YSFReflector-%s", VERSION);

    CTimer watchdogTimer(1000U, 0U, 1500U);

    unsigned char tag[YSF_CALLSIGN_LENGTH];
    unsigned char src[YSF_CALLSIGN_LENGTH];
    unsigned char dst[YSF_CALLSIGN_LENGTH];

    while (keepRunning) {
        unsigned char buffer[200U];
        in_addr address;
        unsigned int port;

        unsigned int len = network.readData(buffer, 200U, address, port);

        std::string remoteAddress = std::string(inet_ntoa(address));

        if (ipBlacklist->isAddressBlacklisted(remoteAddress))
            if (len > 0U) {
                YSFRepeater *rpt = findRepeater(address, port);
                if (::memcmp(buffer, "YSFP", 4U) == 0) {
                    if (rpt == nullptr) {
                        rpt = new YSFRepeater;
                        rpt->m_callsign = std::string((char *) (buffer + 4U), 10U);
                        rpt->m_address = address;
                        rpt->m_port = port;
                        ysfRepeaters.push_back(rpt);
                        network.setCount(ysfRepeaters.size());
                        LogMessage("Adding %s (%s:%u)", rpt->m_callsign.c_str(), ::inet_ntoa(address), port);
                    }
                    rpt->m_timer.start();
                    network.writePoll(address, port);
                } else if (::memcmp(buffer + 0U, "YSFU", 4U) == 0 && rpt != nullptr) {
                    LogMessage("Removing %s (%s:%u) unlinked", rpt->m_callsign.c_str(), ::inet_ntoa(address), port);
                    for (auto it = ysfRepeaters.begin(); it != ysfRepeaters.end(); ++it) {
                        YSFRepeater *itRpt = *it;
                        if (itRpt->m_address.s_addr == rpt->m_address.s_addr && itRpt->m_port == rpt->m_port) {
                            ysfRepeaters.erase(it);
                            delete itRpt;
                            break;
                        }
                    }
                    network.setCount(ysfRepeaters.size());
                } else if (::memcmp(buffer + 0U, "YSFD", 4U) == 0 && rpt != nullptr) {
                    if (!watchdogTimer.isRunning()) {
                        ::memcpy(tag, buffer + 4U, YSF_CALLSIGN_LENGTH);

                        if (::memcmp(buffer + 14U, "          ", YSF_CALLSIGN_LENGTH) != 0)
                            ::memcpy(src, buffer + 14U, YSF_CALLSIGN_LENGTH);
                        else
                            ::memcpy(src, "??????????", YSF_CALLSIGN_LENGTH);

                        if (::memcmp(buffer + 24U, "          ", YSF_CALLSIGN_LENGTH) != 0)
                            ::memcpy(dst, buffer + 24U, YSF_CALLSIGN_LENGTH);
                        else
                            ::memcpy(dst, "??????????", YSF_CALLSIGN_LENGTH);

                        LogMessage("Received data from %10.10s to %10.10s at %10.10s", src, dst, buffer + 4U);
                    } else {
                        if (::memcmp(tag, buffer + 4U, YSF_CALLSIGN_LENGTH) == 0) {
                            bool changed = false;

                            if (::memcmp(buffer + 14U, "          ", YSF_CALLSIGN_LENGTH) != 0 &&
                                ::memcmp(src, "??????????", YSF_CALLSIGN_LENGTH) == 0) {
                                ::memcpy(src, buffer + 14U, YSF_CALLSIGN_LENGTH);
                                changed = true;
                            }

                            if (::memcmp(buffer + 24U, "          ", YSF_CALLSIGN_LENGTH) != 0 &&
                                ::memcmp(dst, "??????????", YSF_CALLSIGN_LENGTH) == 0) {
                                ::memcpy(dst, buffer + 24U, YSF_CALLSIGN_LENGTH);
                                changed = true;
                            }

                            if (changed)
                                LogMessage("Received data from %10.10s to %10.10s at %10.10s", src, dst, buffer + 4U);
                        }
                    }

                    watchdogTimer.start();

                    for (auto ysfRepeater : ysfRepeaters) {
                        if (ysfRepeater->m_address.s_addr != address.s_addr || ysfRepeater->m_port != port)
                            network.writeData(buffer, ysfRepeater->m_address, ysfRepeater->m_port);
                    }

                    if ((buffer[34U] & 0x01U) == 0x01U) {
                        LogMessage("Received end of transmission");
                        watchdogTimer.stop();
                    }
                }
            }

        unsigned int ms = stopWatch.elapsed();
        stopWatch.start();

        pollTimer.clock(ms);
        if (pollTimer.hasExpired()) {
            for (auto ysfRepeater : ysfRepeaters)
                network.writePoll(ysfRepeater->m_address, ysfRepeater->m_port);
            pollTimer.start();
        }

        // Remove any repeaters that haven't reported for a while
        for (auto &ysfRepeater : ysfRepeaters)
            ysfRepeater->m_timer.clock(ms);

        for (auto it = ysfRepeaters.begin(); it != ysfRepeaters.end(); ++it) {
            YSFRepeater *itRpt = *it;
            if (itRpt->m_timer.hasExpired()) {
                LogMessage("Removing %s (%s:%u) disappeared", itRpt->m_callsign.c_str(), ::inet_ntoa(itRpt->m_address),
                           itRpt->m_port);
                ysfRepeaters.erase(it);
                delete itRpt;
                network.setCount(ysfRepeaters.size());
                break;
            }
        }

        watchdogTimer.clock(ms);
        if (watchdogTimer.isRunning() && watchdogTimer.hasExpired()) {
            LogMessage("Network watchdog has expired");
            watchdogTimer.stop();
        }

        dumpTimer.clock(ms);
        if (dumpTimer.hasExpired()) {
            dumpRepeaters();
            dumpTimer.start();
        }

        if (ms < 5U)
            CThread::sleep(5U);
    }

    network.close();

    cliServer->stop();

    ::LogFinalise();
}

void YSFReflector::stop() {
    keepRunning = false;
}

YSFRepeater *YSFReflector::findRepeater(const in_addr &address, unsigned int port) const {
    for (auto ysfRepeater : ysfRepeaters) {
        if (address.s_addr == ysfRepeater->m_address.s_addr && ysfRepeater->m_port == port)
            return ysfRepeater;
    }

    return nullptr;
}

void YSFReflector::dumpRepeaters() const {
    if (ysfRepeaters.empty()) {
        LogMessage("No repeaters/gateways linked");
        return;
    }

    LogMessage("Currently linked repeaters/gateways:");

    for (auto ysfRepeater : ysfRepeaters) {
        std::string callsign = ysfRepeater->m_callsign;
        in_addr address = ysfRepeater->m_address;
        unsigned int port = ysfRepeater->m_port;
        unsigned int timer = ysfRepeater->m_timer.getTimer();
        unsigned int timeout = ysfRepeater->m_timer.getTimeout();
        LogMessage("    %s: %s:%u %u/%u", callsign.c_str(), ::inet_ntoa(address), port, timer, timeout);
    }
}

void YSFReflector::blacklistAdd(std::string address) {
    ipBlacklist->addressAdd(address);
    ipBlacklist->saveToFile(configuration->getNetworkBlacklistFile());

    size_t count = ipBlacklist->countBlacklistedAddresses();
    const char *address_str = address.c_str();
    LogInfo("[ipBlacklist] Add: %s - Total blacklisted addresses: %d", address_str, count);
}

void YSFReflector::blacklistRemove(std::string address) {
    ipBlacklist->addressRemove(address);
    ipBlacklist->saveToFile(configuration->getNetworkBlacklistFile());

    size_t count = ipBlacklist->countBlacklistedAddresses();
    const char *address_str = address.c_str();
    LogInfo("[ipBlacklist] Remove: %s - Total blacklisted addresses: %d", address_str, count);
}
