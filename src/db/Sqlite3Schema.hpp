#pragma once
#include <string>

namespace memo {

namespace MemoTable {
    const std::string kName = "Memo";

    namespace att {
        const std::string kId = "id";
        const std::string kTitle = "title";
        const std::string kDescription = "description";
        const std::string kTimestamp = "timestamp";
    } // namespace att

    namespace cmd {
        std::string Drop();

        std::string Create();
    } // namespace cmd
} // namespace MemoTable

namespace TagTable {
    const std::string kName = "Tag";

    namespace att {
        const std::string kId = "id";
        const std::string kName = "name";
        const std::string kColor = "color";
        const std::string kTimestamp = "timestamp";
    } // namespace att

    namespace cmd {
        std::string Drop();

        std::string Create();
    } // namespace cmd
} // namespace TagTable

namespace TaggedTable {
    const std::string kName = "Tagged";

    namespace att {
        const std::string kMemoId = "memoId";
        const std::string kTagId = "tagId";
    } // namespace att

    namespace cmd {
        std::string Drop();

        std::string Create();
    } // namespace cmd
} // namespace TaggedTable

} // namespace memo