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

#ifndef __YSF_REFLECTOR__YFS_REPEATER__H
#define __YSF_REFLECTOR__YFS_REPEATER__H

#include <cstdio>
#include <string>
#include <vector>
#include <netdb.h>
#include <ctime>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Timer.h"
#include "configuration.hpp"

class YSFRepeater {

public:

    YSFRepeater();

    std::string m_callsign;
    in_addr m_address;
    unsigned int m_port;
    CTimer m_timer;
};

#endif
