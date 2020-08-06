#pragma once
#include <vector>

namespace memo {
namespace service {

class Process;

class Service
{
public:
    virtual ~Service() = default;

    virtual int getId() const = 0;

    virtual void enable() = 0;

    virtual void disable() = 0;
};

} // namespace service
} // namespace memo
