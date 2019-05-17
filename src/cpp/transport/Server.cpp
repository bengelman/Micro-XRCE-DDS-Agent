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

#include <uxr/agent/transport/Server.hpp>
#include <uxr/agent/config.hpp>
#include <uxr/agent/processor/Processor.hpp>
#include <uxr/agent/Root.hpp>
#include <functional>

#define RECEIVE_TIMEOUT 1

namespace eprosima {
namespace uxr {

template<typename T>
Server<T>::Server(MiddlewareKind middleware_kind)
    : running_cond_(false)
{}

template<typename T>
Server<T>::~Server()
{
}

template<typename T>
bool Server<T>::run()
{
    /* Init server. */
    if (!init())
    {
        return false;
    }

    /* Scheduler initialization. */
    input_scheduler_.init();
    output_scheduler_.init();

    /* Thread initialization. */
    running_cond_ = true;
    receiver_thread_.reset(new std::thread(std::bind(&Server::receiver_loop, this)));
    sender_thread_.reset(new std::thread(std::bind(&Server::sender_loop, this)));
    processing_thread_.reset(new std::thread(std::bind(&Server::processing_loop, this)));
    heartbeat_thread_.reset(new std::thread(std::bind(&Server::heartbeat_loop, this)));

    return true;
}

template<typename T>
bool Server<T>::stop()
{
    running_cond_ = false;
    input_scheduler_.deinit();
    output_scheduler_.deinit();
    if (receiver_thread_ && receiver_thread_->joinable())
    {
        receiver_thread_->join();
    }
    if (sender_thread_ && sender_thread_->joinable())
    {
        sender_thread_->join();
    }
    if (processing_thread_ && processing_thread_->joinable())
    {
        processing_thread_->join();
    }
    if (heartbeat_thread_ && heartbeat_thread_->joinable())
    {
        heartbeat_thread_->join();
    }

    bool rv = true;
#ifdef PROFILE_DISCOVERY
    rv &= close_discovery();
#endif
#ifdef PROFILE_P2P
    rv &= close_p2p();
#endif
    rv &= close();
    return rv;
}

template<typename T>
bool Server<T>::load_config_file(
        const std::string& path)
{
    return Root::instance().load_config_file(path);
}

#ifdef PROFILE_DISCOVERY
template<typename T>
bool Server<T>::enable_discovery(
        uint16_t discovery_port)
{
    bool rv = false;
    if (running_cond_)
    {
        rv = init_discovery(discovery_port);
    }
    return rv;
}

template<typename T>
bool Server<T>::disable_discovery()
{
    return close_discovery();
}
#endif

#ifdef PROFILE_P2P
template<typename T>
bool Server<T>::enable_p2p(
        uint16_t p2p_port)
{
    bool rv = false;
    if (running_cond_)
    {
        rv = init_p2p(p2p_port);
    }
    return rv;
}

template<typename T>
bool Server<T>::disable_p2p()
{
    return close_p2p();
}
#endif

template<typename T>
void Server<T>::push_output_packet(
        OutputPacket<T> output_packet)
{
    if (output_packet.destination && output_packet.message)
    {
        output_scheduler_.push(std::move(output_packet), 0);
    }
}

template<typename T>
void Server<T>::receiver_loop()
{
    InputPacket<T> input_packet;
    while (running_cond_)
    {
        if (recv_message(input_packet, RECEIVE_TIMEOUT))
        {
            input_scheduler_.push(std::move(input_packet), 0);
        }
    }
}

template<typename T>
void Server<T>::sender_loop()
{
    OutputPacket<T> output_packet;
    while (running_cond_)
    {
        if (output_scheduler_.pop(output_packet))
        {
            send_message(output_packet);
        }
    }
}

template<typename T>
void Server<T>::processing_loop()
{
    InputPacket<T> input_packet;
    while (running_cond_)
    {
        if (input_scheduler_.pop(input_packet))
        {
            processor_->process_input_packet(std::move(input_packet));
        }
    }
}

template<typename T>
void Server<T>::heartbeat_loop()
{
    while (running_cond_)
    {
        processor_->check_heartbeats();
        std::this_thread::sleep_for(std::chrono::milliseconds(HEARTBEAT_PERIOD));
    }
}

} // namespace uxr
} // namespace eprosima
