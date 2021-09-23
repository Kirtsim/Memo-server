#include "ModelTestHelperFunctions.hpp"
#include "model/Memo.hpp"
#include "model/Tag.hpp"
#include "db/Sqlite3Wrapper.hpp"
#include "db/Tools.hpp"

namespace memo::test {

model::MemoPtr CreateMemo(const test::MemoValues& values)
{
    auto memo = std::make_shared<model::Memo>();
    memo->setId(values.id);
    memo->setTitle(values.title);
    memo->setDescription(values.description);
    memo->setTimestamp(values.timestamp);
    return memo;
}

model::TagPtr CreateTag(const test::TagValues& values)
{
    auto tag = std::make_shared<model::Tag>();
    tag->setId(values.id);
    tag->setName(values.name);
    tag->setColor(tools::IntToColor(values.color));
    tag->setTimestamp(values.timestamp);
    return tag;
}

test::MemoValues MemoToValues(const model::Memo& memo)
{
    test::MemoValues values;
    values.id = memo.id();
    values.title = memo.title();
    values.description = memo.description();
    values.timestamp = memo.timestamp();
    return values;
}

test::TagValues TagToValues(const model::Tag& tag)
{
    test::TagValues values;
    values.id = tag.id();
    values.name = tag.name();
    values.color = tools::ColorToInt(tag.color());
    values.timestamp = tag.timestamp();
    return values;
}

std::vector<std::string> ToStringVector(const model::Memo& memo)
{
    return {
        std::to_string(memo.id()),
        memo.title(),
        memo.description(),
        std::to_string(memo.timestamp()),
    };
}

std::vector<std::string> ToStringVector(const model::Tag& tag)
{
    return {
        std::to_string(tag.id()),
        tag.name(),
        std::to_string(tools::ColorToInt(tag.color())),
        std::to_string(tag.timestamp()),
    };
}

} // namespace memo::test
