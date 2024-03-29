#pragma once
#include "server/process/IProcess.hpp"
#include <memory>
#include <vector>

namespace memo {

class IService
{
public:
    using Ptr = std::shared_ptr<IService>;

    virtual ~IService() = default;

    virtual bool executeProcess(IProcess* process) = 0;

    virtual int getId() const = 0;

    virtual void enable() = 0;

    virtual void disable() = 0;
};

} // namespace memo
