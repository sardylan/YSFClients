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

#ifndef __YSF_REFLECTOR__CLI_SERVER__H
#define __YSF_REFLECTOR__CLI_SERVER__H

#include <string>
#include <thread>

namespace ysf {

    struct CliCallback {

        virtual void blacklistAdd(std::string address) = 0;

        virtual void blacklistRemove(std::string address) = 0;
    };

    class CliServer {

    public:

        explicit CliServer(CliCallback *callback);

        ~CliServer();

        bool start();

        void stop();

        const std::string &getListenAddress() const;

        void setListenAddress(const std::string &listenAddress);

        uint16_t getListenPort() const;

        void setListenPort(uint16_t listenPort);

        const std::string &getUsername() const;

        void setUsername(const std::string &username);

        const std::string &getPassword() const;

        void setPassword(const std::string &password);

    private:

        CliCallback *callback;

        std::string listenAddress;
        uint16_t listenPort;

        bool keepRunning;
        std::thread *thread;

        int sock;

        std::string username;
        std::string password;

        void loop();

        bool parseCommand(char *);
    };
};

#endif
