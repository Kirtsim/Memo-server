#pragma once
#include <string>
#include <vector>

namespace memo::model {

class Memo
{
public:
    unsigned long id() const;
    void setId(unsigned long Id);

    const std::string& title() const;
    void setTitle(const std::string& title);

    const std::string& description() const;
    void setDescription(const std::string& description);

    const std::vector<unsigned long>& tagIds() const;
    void setTagIds(const std::vector<unsigned long>& tagId);
    void addTagId(unsigned long tagId);

    unsigned long timestamp() const;
    void setTimestamp(unsigned long timestamp);

private:
    unsigned long id_;
    std::string title_;
    std::string description_;
    std::vector<unsigned long> tagIds_;
    unsigned long timestamp_;
};

}


