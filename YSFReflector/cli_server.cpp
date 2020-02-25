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
#include <unistd.h>
#include <cstring>
#include <vector>

#include "cli_server.hpp"

#define YSF_CLI_SOCKET_BUFFER 1024

using namespace ysf;

CliServer::CliServer(CliCallback *callback) {
    this->callback = callback;

    this->listenAddress = "0.0.0.0";
    this->listenPort = 42023;

    this->keepRunning = false;
    this->thread = nullptr;

    this->sock = 0;

    this->username = "";
    this->password = "";
}

CliServer::~CliServer() {
    delete thread;
}

const std::string &CliServer::getListenAddress() const {
    return listenAddress;
}

void CliServer::setListenAddress(const std::string &value) {
    CliServer::listenAddress = value;
}

uint16_t CliServer::getListenPort() const {
    return listenPort;
}

void CliServer::setListenPort(uint16_t value) {
    CliServer::listenPort = value;
}

const std::string &CliServer::getUsername() const {
    return username;
}

void CliServer::setUsername(const std::string &value) {
    CliServer::username = value;
}

const std::string &CliServer::getPassword() const {
    return password;
}

void CliServer::setPassword(const std::string &value) {
    CliServer::password = value;
}

bool CliServer::start() {
    sockaddr_in address;

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(listenAddress.c_str());
    address.sin_port = htons(listenPort);

    int res = bind(sock, (sockaddr *) &address, sizeof(address));
    if (res == -1) {
        return false;
    }

    keepRunning = true;
    thread = new std::thread(&CliServer::loop, std::ref(*this));

    return true;
}

void CliServer::loop() {
    char buffer[YSF_CLI_SOCKET_BUFFER];
    struct sockaddr remote_address;
    socklen_t addrlen = sizeof(remote_address);

    while (keepRunning) {
        bzero(buffer, YSF_CLI_SOCKET_BUFFER);

        ssize_t receivedBytes = recvfrom(sock, buffer, YSF_CLI_SOCKET_BUFFER, 0, &remote_address, &addrlen);
        if (receivedBytes == 0)
            continue;

        bool result = parseCommand(buffer);

        if (result) {
            sendto(sock, "OK", 2, 0, &remote_address, addrlen);
        } else {
            sendto(sock, "ERROR", 5, 0, &remote_address, addrlen);
        }
    }
}

void CliServer::stop() {
    keepRunning = false;

    if (sock > 0)
        close(sock);
    sock = 0;

    if (thread != nullptr)
        thread->join();
    delete thread;
}

bool CliServer::parseCommand(char *buffer) {
    char *save = nullptr;

    char *username;
    char *password;
    char *command;
    char *arg;
    std::vector<char *> args;

    username = strtok_r(buffer, " ", &save);
    if (username == nullptr)
        return false;
    if (std::strcmp(username, this->username.c_str()) != 0)
        return false;

    password = strtok_r(nullptr, " ", &save);
    if (password == nullptr)
        return false;
    if (std::strcmp(password, this->password.c_str()) != 0)
        return false;

    command = strtok_r(nullptr, " ", &save);
    if (command == nullptr)
        return false;

    do {
        arg = strtok_r(nullptr, " ", &save);
        args.push_back(arg);
    } while (arg != nullptr);

    if (std::strcmp(command, "BLACKLIST") == 0) {
        if (args.empty())
            return false;

        if (std::strcmp(args[0], "ADD") == 0) {
            if (args.size() < 2)
                return false;
            std::string address = std::string(args[1]);
            callback->blacklistAdd(address);
            return true;
        } else if (std::strcmp(args[0], "REMOVE") == 0) {
            if (args.size() < 2)
                return false;
            std::string address = std::string(args[1]);
            callback->blacklistRemove(address);
            return true;
        } else {
            return false;
        }
    }

    return false;
}
