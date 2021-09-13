#include "model/Memo.hpp"

namespace memo::model {

unsigned long Memo::id() const
{
    return id_;
}

void Memo::setId(unsigned long id)
{
    id_ = id;
}

const std::string& Memo::title() const
{
    return title_;
}

void Memo::setTitle(const std::string& title)
{
    title_ = title;
}

const std::string& Memo::description() const
{
    return description_;
}

void Memo::setDescription(const std::string& description)
{
    description_ = description;
}

unsigned long Memo::timestamp() const
{
    return timestamp_;
}

void Memo::setTimestamp(unsigned long timestamp)
{
    timestamp_ = timestamp;
}

const std::vector<unsigned long>& Memo::tagIds() const
{
    return tagIds_;
}

void Memo::setTagIds(const std::vector<unsigned long>& tagIds)
{
    tagIds_ = tagIds;
}

void Memo::addTagId(unsigned long tagId)
{
    tagIds_.emplace_back(tagId);
}

} // namespace memo::model
