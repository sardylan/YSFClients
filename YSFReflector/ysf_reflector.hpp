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


#ifndef __YSF_REFLECTOR__YSF_REFLECTOR__H
#define __YSF_REFLECTOR__YSF_REFLECTOR__H

#include <cstdio>
#include <string>
#include <vector>

#if !defined(_WIN32) && !defined(_WIN64)

#include <netdb.h>
#include <ctime>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#else
#include <winsock.h>
#endif

#include "Timer.h"
#include "configuration.hpp"
#include "ip_blacklist.hpp"
#include "cli_server.hpp"
#include "ysf_repeater.hpp"

using namespace ysf;

void signalHandler(int);

class YSFReflector : public CliCallback {

public:

    static YSFReflector *getInstance();

    void run(const std::string &);

    void stop();

    void blacklistAdd(std::string address) override;

    void blacklistRemove(std::string address) override;

private:

    static YSFReflector *instance;

    bool keepRunning;

    Configuration *configuration;
    IpBlacklist *ipBlacklist;
    CliServer *cliServer;
    std::vector<YSFRepeater *> ysfRepeaters;

    YSFReflector();

    ~YSFReflector();

    YSFRepeater *findRepeater(const in_addr &, unsigned int) const;

    void dumpRepeaters() const;

};

#endif
