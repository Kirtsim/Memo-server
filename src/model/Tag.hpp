#pragma once
#include "model/Color.hpp"
#include <string>

namespace memo::model {

class Tag
{
public:
    unsigned long id() const;

    void setId(unsigned long id);

    const std::string& name() const;

    void setName(const std::string& name);

    const Color& color() const;

    void setColor(const Color& color);

    unsigned long timestamp() const;

    void setTimestamp(unsigned long timestamp);

private:
    unsigned long id_ = -1ul;
    std::string name_;
    Color color_;
    unsigned long timestamp_ = 0;
};

bool operator==(const Tag& first, const Tag& second);

bool operator!=(const Tag& first, const Tag& second);
} // namespace memo::model
