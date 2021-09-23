#include "model/Tag.hpp"

namespace memo::model {

unsigned long Tag::id() const
{
    return id_;
}

void Tag::setId(unsigned long id)
{
    id_ = id;
}

const std::string& Tag::name() const
{
    return name_;
}

void Tag::setName(const std::string& name)
{
    name_ = name;
}

const Color& Tag::color() const
{
    return color_;
}

void Tag::setColor(const Color& color)
{
    color_ = color;
}

unsigned long Tag::timestamp() const
{
    return timestamp_;
}

void Tag::setTimestamp(unsigned long timestamp)
{
    timestamp_ = timestamp;
}

bool operator==(const Tag& first, const Tag& second)
{
    return &first == &second || (
           first.id() == second.id() &&
           first.name() == second.name() &&
           first.color() == second.color() &&
           first.timestamp() == second.timestamp() );
}


bool operator!=(const Tag& first, const Tag& second)
{
    return !(first == second);
}

} // namespace memo::model