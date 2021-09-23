#pragma once
#include "model/Tag.hpp"
#include <string>
#include <vector>

namespace memo::model {

class Memo
{
public:
    unsigned long id() const;

    void setId(unsigned long id);

    const std::string& title() const;

    void setTitle(const std::string& title);

    const std::string& description() const;

    void setDescription(const std::string& description);

    unsigned long timestamp() const;

    void setTimestamp(unsigned long timestamp);

    const std::vector<unsigned long>& tagIds() const;

    void setTagIds(const std::vector<unsigned long>& tagIds);

    void addTagId(unsigned long tag);

private:
    unsigned long id_ = -1ul;
    std::string title_;
    std::string description_;
    std::vector<unsigned long> tagIds_;
    unsigned long timestamp_ = 0;
};

bool operator==(const Memo& first, const Memo& second);

bool operator!=(const Memo& first, const Memo& second);

} // namespace memo::model
