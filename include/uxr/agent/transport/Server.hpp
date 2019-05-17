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

#ifndef UXR_AGENT_TRANSPORT_SERVER_HPP_
#define UXR_AGENT_TRANSPORT_SERVER_HPP_

#include <uxr/agent/transport/SessionManager.hpp>
#include <uxr/agent/transport/EndPoint.hpp>
#include <uxr/agent/scheduler/FCFSScheduler.hpp>
#include <uxr/agent/message/Packet.hpp>
#include <uxr/agent/processor/Processor.hpp>
#include <uxr/agent/agent_dll.hpp>

#include <thread>

namespace eprosima {
namespace uxr {

class Processor;

template<typename T>
class Server
{
    friend class Processor;
public:
    Server(MiddlewareKind middleware_kind);

    virtual ~Server();

    UXR_AGENT_EXPORT bool run();

    UXR_AGENT_EXPORT bool stop();

    UXR_AGENT_EXPORT bool load_config_file(
            const std::string& path);

#ifdef PROFILE_DISCOVERY
    UXR_AGENT_EXPORT bool enable_discovery(
            uint16_t discovery_port = DISCOVERY_PORT);

    UXR_AGENT_EXPORT bool disable_discovery();
#endif

#ifdef PROFILE_P2P
    UXR_AGENT_EXPORT bool enable_p2p(
            uint16_t p2p_port);

    UXR_AGENT_EXPORT bool disable_p2p();
#endif

private:
    void push_output_packet(
            OutputPacket<T> output_packet);

    virtual bool init() = 0;

    virtual bool close() = 0;

#ifdef PROFILE_DISCOVERY
    virtual bool init_discovery(
            uint16_t discovery_port) = 0;

    virtual bool close_discovery() = 0;
#endif

#ifdef PROFILE_P2P
    virtual bool init_p2p(
            uint16_t p2p_port) = 0;

    virtual bool close_p2p() = 0;
#endif

    virtual bool recv_message(
            InputPacket<T>& input_packet,
            int timeout) = 0;

    virtual bool send_message(
            OutputPacket<T> output_packet) = 0;

    virtual int get_error() = 0;

    void receiver_loop();

    void sender_loop();

    void processing_loop();

    void heartbeat_loop();

protected:
    Processor* processor_;

private:
    std::unique_ptr<std::thread> receiver_thread_;
    std::unique_ptr<std::thread> sender_thread_;
    std::unique_ptr<std::thread> processing_thread_;
    std::unique_ptr<std::thread> heartbeat_thread_;
    std::atomic<bool> running_cond_;

    FCFSScheduler<InputPacket<T>> input_scheduler_;
    FCFSScheduler<OutputPacket<T>> output_scheduler_;

    SessionManager<T> session_manager_;
};

} // namespace uxr
} // namespace eprosima

#endif // UXR_AGENT_TRANSPORT_SERVER_HPP_
