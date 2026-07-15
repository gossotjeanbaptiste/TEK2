/*
** EPITECH PROJECT, 2026
** Network.cpp
** File description:
** Non-blocking TCP client for zappy_gui
*/
#include "gui/Network.hpp"
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <poll.h>
#include <iostream>

Network::Network() : _fd(-1), _connected(false), _connecting(false) {}

Network::~Network()
{
    if (_fd >= 0)
        close(_fd);
}

bool Network::connect(const std::string &host, int port)
{
    struct addrinfo hints{};
    struct addrinfo *res = nullptr;

    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    std::string portStr = std::to_string(port);
    int err = getaddrinfo(host.c_str(), portStr.c_str(), &hints, &res);
    if (err != 0)
        throw std::runtime_error(
            std::string("getaddrinfo: ") + gai_strerror(err));

    int fd = -1;
    for (struct addrinfo *p = res; p != nullptr; p = p->ai_next) {
        fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (fd < 0)
            continue;

        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);

        int ret = ::connect(fd, p->ai_addr, p->ai_addrlen);
        if (ret == 0) {
            _fd = fd;
            _connecting = false;
            break;
        }
        if (errno == EINPROGRESS) {
            _fd = fd;
            _connecting = true;
            break;
        }
        close(fd);
        fd = -1;
    }
    freeaddrinfo(res);

    if (_fd < 0)
        throw std::runtime_error("Unable to connect to " + host + ":" + portStr);

    if (!_connecting)
        _connected = true;
    return true;
}

void Network::poll()
{
    if (!_connected && !_connecting)
        return;

    if (_connecting) {
        struct pollfd pfd{};
        pfd.fd = _fd;
        pfd.events = POLLOUT;
        int ret = ::poll(&pfd, 1, 0);

        if (ret > 0) {
            int err = 0;
            socklen_t len = sizeof(err);
            if (getsockopt(_fd, SOL_SOCKET, SO_ERROR, &err, &len) < 0 || err != 0) {
                std::cerr << "Network: connection failed: " << std::strerror(err) << std::endl;
                close(_fd);
                _fd = -1;
                _connected = false;
                _connecting = false;
                return;
            }
            _connecting = false;
            _connected = true;
        } else if (ret < 0) {
            std::cerr << "Network: poll error: " << std::strerror(errno) << std::endl;
            close(_fd);
            _fd = -1;
            _connected = false;
            _connecting = false;
            return;
        }

        if (_connecting)
            return;
    }

    flushSendBuffer();

    char buf[4096];
    for (;;) {
        ssize_t n = recv(_fd, buf, sizeof(buf), 0);
        if (n > 0) {
            _recvBuffer.append(buf, static_cast<size_t>(n));
            continue;
        }
        if (n == 0) {
            std::cerr << "Network: server closed the connection" << std::endl;
            close(_fd);
            _fd = -1;
            _connected = false;
            return;
        }
        // n < 0
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            break;
        if (errno == EINTR)
            continue;
        std::cerr << "Network: recv error: " << std::strerror(errno) << std::endl;
        close(_fd);
        _fd = -1;
        _connected = false;
        return;
    }

    size_t pos;
    while ((pos = _recvBuffer.find('\n')) != std::string::npos) {
        _lines.push_back(_recvBuffer.substr(0, pos));
        _recvBuffer.erase(0, pos + 1);
    }
}

void Network::send(const std::string &msg)
{
    _sendBuffer += msg;
    if (_sendBuffer.empty() || _sendBuffer.back() != '\n')
        _sendBuffer += '\n';
    flushSendBuffer();
}

void Network::flushSendBuffer()
{
    if (!_connected || _sendBuffer.empty())
        return;

    while (!_sendBuffer.empty()) {
        ssize_t n = ::send(_fd, _sendBuffer.data(), _sendBuffer.size(), 0);
        if (n > 0) {
            _sendBuffer.erase(0, static_cast<size_t>(n));
            continue;
        }
        if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
            break;
        if (n < 0 && errno == EINTR)
            continue;
        std::cerr << "Network: send error: " << std::strerror(errno) << std::endl;
        close(_fd);
        _fd = -1;
        _connected = false;
        return;
    }
}

bool Network::hasLine() const
{
    return !_lines.empty();
}

std::string Network::popLine()
{
    std::string line = _lines.front();
    _lines.pop_front();
    return line;
}

bool Network::isConnected() const
{
    return _connected;
}