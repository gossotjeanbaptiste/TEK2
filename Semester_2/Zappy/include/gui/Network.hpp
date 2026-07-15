/*
** EPITECH PROJECT, 2026
** Network.hpp
** File description:
** TCP client for zappy_gui
*/
#pragma once
 
#include <string>
#include <deque>
 
class Network {
    public:
        Network();
        ~Network();
 
        Network(const Network &) = delete;
        Network &operator=(const Network &) = delete;
 
        bool connect(const std::string &host, int port);
    
        void poll();
 
        void send(const std::string &msg);
 
        bool hasLine() const;
        std::string popLine();
 
        bool isConnected() const;
 
    private:
        void flushSendBuffer();

        int         _fd;
        bool        _connected;
        bool        _connecting;
        std::string _recvBuffer;
        std::string _sendBuffer;
        std::deque<std::string> _lines;
};
