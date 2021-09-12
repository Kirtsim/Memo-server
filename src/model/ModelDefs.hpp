#pragma once
#include <memory>

namespace memo::model {

class Memo;
class Tag;

using MemoPtr = std::shared_ptr<Memo>;
using TagPtr  = std::shared_ptr<Tag>;

} // namespace memo::model
