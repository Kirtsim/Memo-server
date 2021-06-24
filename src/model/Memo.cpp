#include "model/Memo.hpp"

namespace memo::model {

unsigned long Memo::id() const
{
    return id_;
}

const std::string& Memo::title() const
{
    return title_;
}

const std::string& Memo::description() const
{
    return description_;
}

const std::vector<unsigned long>& Memo::tagIds() const
{
    return tagIds_;
}

unsigned long Memo::timestamp() const
{
    return timestamp_;
}

void Memo::setId(const unsigned long id)
{
    id_ = id;
}

void Memo::setTitle(const std::string& title)
{
    title_ = title;
}

void Memo::setDescription(const std::string& description)
{
    description_ = description;
}

void Memo::setTagIds(const std::vector<unsigned long>& tagIds)
{
    tagIds_ = tagIds;
}

void Memo::addTagId(const unsigned long tagId)
{
    tagIds_.emplace_back(tagId);
}

void Memo::setTimestamp(const unsigned long timestamp)
{
    timestamp_ = timestamp;
}

} // namespace memo::model