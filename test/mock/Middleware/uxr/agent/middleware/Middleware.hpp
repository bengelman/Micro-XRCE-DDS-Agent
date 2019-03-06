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

#ifndef UXR_AGENT_MIDDLEWARE_MIDDLEWARE_HPP_
#define UXR_AGENT_MIDDLEWARE_MIDDLEWARE_HPP_

#include <string>
#include <cstdint>
#include <cstddef>
#include <vector>
#include <functional>

namespace eprosima {
namespace uxr {

enum MiddlewareKind : uint8_t
{
#ifdef PROFILE_FAST_MIDDLEWARE
    FAST_MIDDLEWARE,
#endif

#ifdef PROFILE_CED_MIDDLEWARE
    CED_MIDDLEWARE,
#endif
};

typedef std::function<void ()> OnNewData;

class Middleware
{
public:
    Middleware() = default;
    virtual ~Middleware() = default;
};

} // namespace uxr
} // namespace eprosima

#endif // UXR_AGENT_MIDDLEWARE_MIDDLEWARE_HPP_
