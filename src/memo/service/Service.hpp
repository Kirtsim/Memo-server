#pragma once
#include "memo/service/Process.hpp"
#include <memory>
#include <vector>

namespace memo {
namespace service {

class Service
{
public:
    using Ptr = std::shared_ptr<Service>;

    virtual ~Service() = default;

    virtual bool executeProcess(Process* process) = 0;

    virtual int getId() const = 0;

    virtual void enable() = 0;

    virtual void disable() = 0;
};

} // namespace service
} // namespace memo
