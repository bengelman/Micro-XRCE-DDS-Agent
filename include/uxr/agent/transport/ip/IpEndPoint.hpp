// Copyright 2018 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef UXR_AGENT_TRANSPORT_IP_ENDPOINT_HPP_
#define UXR_AGENT_TRANSPORT_IP_ENDPOINT_HPP_

#include <uxr/agent/transport/EndPoint.hpp>

#include <cstdint>

namespace eprosima {
namespace uxr {

class Ipv4EndPoint : public EndPoint
{
public:
    Ipv4EndPoint(uint32_t addr, uint16_t port) : addr_(addr), port_(port) {}
    ~Ipv4EndPoint() = default;

    friend bool operator<(const Ipv4EndPoint& lhs, const Ipv4EndPoint& rhs)
    {
        return (lhs.addr_ < rhs.addr_)
                ? true : (lhs.addr_ > rhs.addr_)
                  ? false : (lhs.port_ < rhs.port_)
                    ? true : false;
    }

    uint32_t get_addr() const { return addr_; }
    uint16_t get_port() const { return port_; }

private:
    uint32_t addr_;
    uint16_t port_;
};

class Ipv6EndPoint : public EndPoint
{
public:
    Ipv6EndPoint(uint32_t addr, uint16_t port) : addr_(addr), port_(port) {}
    ~Ipv6EndPoint() = default;

    uint64_t get_addr() const { return addr_; }
    uint16_t get_port() const { return port_; }

private:
    uint64_t addr_;
    uint16_t port_;
};

} // namespace uxr
} // namespace eprosima

#endif // UXR_AGENT_TRANSPORT_IP_ENDPOINT_HPP_