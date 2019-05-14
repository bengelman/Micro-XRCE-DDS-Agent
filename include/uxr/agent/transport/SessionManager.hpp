// Copyright 2019 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#ifndef UXR_AGENT_TRANSPORT_SESSION_MANAGER_HPP_
#define UXR_AGENT_TRANSPORT_SESSION_MANAGER_HPP_

#include <uxr/agent/types/XRCETypes.hpp>

#include <map>
#include <mutex>

namespace eprosima {
namespace uxr {

template<typename T>
class SessionManager
{
public:
    void create_session(
            const T& source,
            const dds::xrce::ClientKey& client_key);

    void delete_session(
            const T& source);

    bool get_endpoint(
            const dds::xrce::ClientKey& client_key,
            T& endpoint) const;

    bool get_client_key(
            const T& endpoint,
            dds::xrce::ClientKey& client_key) const;

private:
    std::map<T, uint32_t> endpoint_to_client_;
    std::map<uint32_t, T> client_to_endpoint_;
    std::mutex mtx_;
};

template<typename T>
inline void SessionManager<T>::create_session(
        const T& endpoint,
        const dds::xrce::ClientKey& client_key)
{
    uint32_t client_id = uint32_t(client_key.at(0) + (client_key.at(1) << 8) + (client_key.at(2) << 16) + (client_key.at(3) << 24));

    /* Update maps. */
    std::lock_guard<std::mutex> lock(mtx_);
    auto it_client = client_to_endpoint_.find(client_id);
    if (it_client != client_to_endpoint_.end())
    {
        endpoint_to_client_.erase(it_client->second);
        it_client->second = endpoint;
    }
    else
    {
        client_to_endpoint_.emplace(client_id, endpoint);
    }

    auto it_source = endpoint_to_client_.find(endpoint);
    if (it_source != endpoint_to_client_.end())
    {
        it_source->second = client_id;
    }
    else
    {
        endpoint_to_client_.emplace(endpoint, client_id);
    }
}

template<typename T>
inline void SessionManager<T>::delete_session(
        const T& endpoint)
{
    /* Update maps. */
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = endpoint_to_client_.find(endpoint);
    if (it != endpoint_to_client_.end())
    {
        client_to_endpoint_.erase(it->second);
        endpoint_to_client_.erase(it->first);
    }
}

template<typename T>
inline bool SessionManager<T>::get_endpoint(
        const dds::xrce::ClientKey& client_key,
        T& endpoint) const
{
    bool rv = false;
    uint32_t client_id = uint32_t(client_key.at(0) + (client_key.at(1) << 8) + (client_key.at(2) << 16) + (client_key.at(3) <<24));
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = client_to_endpoint_.find(client_id);
    if (it != client_to_endpoint_.end())
    {
        endpoint = it->second;
        rv = true;
    }
    return rv;
}

template<typename T>
inline bool SessionManager<T>::get_client_key(
        const T& endpoint,
        dds::xrce::ClientKey& client_key) const
{
    bool rv = false;
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = endpoint_to_client_.find((uint64_t(endpoint->get_addr()) << 16) | endpoint->get_port());
    if (it != endpoint_to_client_.end())
    {
        client_key.at(0) = uint8_t(it->second & 0x000000FF);
        client_key.at(1) = uint8_t((it->second & 0x0000FF00) >> 8);
        client_key.at(2) = uint8_t((it->second & 0x00FF0000) >> 16);
        client_key.at(3) = uint8_t((it->second & 0xFF000000) >> 24);
        rv = true;
    }
    return rv;
}

} // namespace uxr
} // namespace eprosima

#endif // UXR_AGENT_TRANSPORT_SESSION_MANAGER_HPP_
