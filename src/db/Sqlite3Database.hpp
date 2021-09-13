#pragma once
#include "db/IDatabase.hpp"
#include <memory>

namespace memo {

class ISqlite3Wrapper;

class Sqlite3Database : public IDatabase
{
public:
    explicit Sqlite3Database(std::unique_ptr<ISqlite3Wrapper> wrapper);

    ~Sqlite3Database() override = default;

    std::vector<model::MemoPtr> listMemos(const MemoSearchFilter& filter) override;

    std::vector<model::TagPtr> listTags(const TagSearchFilter& filter) override;

    bool updateMemo(const model::MemoPtr& memo) override;

    bool updateTag(const model::TagPtr& tag) override;

    bool insertMemo(const model::MemoPtr& memo) override;

    bool insertTag(const model::TagPtr& tag) override;

    bool deleteMemo(const model::MemoPtr& memo) override;

    bool deleteTag(const model::TagPtr& tag) override;

private:
    bool beginTransaction();

    bool commitTransaction();

    bool rollback();

private:
    std::unique_ptr<ISqlite3Wrapper> sqlite3_;
};

} // namespace memo
