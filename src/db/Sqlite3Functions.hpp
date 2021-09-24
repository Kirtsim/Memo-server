#pragma once
#include "model/ModelDefs.hpp"
#include <string>
#include <vector>

namespace memo {

class ISqlite3Wrapper;
struct TagSearchFilter;

/// @brief Executes a given query using the provided sqlite3 db wrapper and converts returned values to model::Tags.
///        This function assumes the values returned by the query are solely the attributes of the Tag table, that all
///        the attributes are present and that they're in the correct order: id, name, color, timestamp.
/// @param query SELECT query that returns Tag rows with all Tag's attributes.
/// @param sqlite3 Wrapper object for the sqlite3 db.
/// @return A vector of model::Tag's built from the values returned by the query.
std::vector<model::TagPtr> SelectTags(const std::string& query, ISqlite3Wrapper& sqlite3);

/// @brief Updates a single row in the 'Memo' table. The function uses the ID of the provided Memo to identify the row
///        that needs to be updated and attempts to override all its attributes with the data in the given Memo.
/// @param memo Memo to be updated.
/// @param sqlite3 Wrapper object for sqlite3 db.
/// @return true if update succeeded (even if no row was updated), false if the operation failed.
bool UpdateMemoTable(const model::Memo& memo, ISqlite3Wrapper& sqlite3);

/// @brief Updates a single row in the 'Tag' table. The function uses the Tag ID to identify the row that needs to be
///        updated and attempts to override all the remaining attributes in the row with the ones in the Tag.
/// @param tag Tag to be updated.
/// @param sqlite3 Wrapper object for sqlite3 db.
/// @return true if update succeeded (even if no row was updated), false if the operation failed.
bool UpdateTagTable(const model::Tag& tag, ISqlite3Wrapper& sqlite3);

/// @brief Performs a SELECT query on the 'Tagged' table, selecting all Tag IDs for which Memo ID is the same as the one
///        passed to this function. In other words, this function returns IDs of all Tags that belong to a Memo.
/// @param memoId ID of Memo.
/// @param tagIds IDs of Tags.
/// @param sqlite3 Wrapper object for sqlite3 db.
/// @return true if selection succeeded (even if no ids were selected), false if the operation failed.
bool SelectMemoTagIds(unsigned long memoId, std::vector<unsigned long>& tagIds, ISqlite3Wrapper& sqlite3);

/// @brief Inserts rows into the 'Tagged' table where inserted row is a combination of the given memo ID and tag IDs.
///        In simple terms, this represents a Memo being tagged with new Tags.
/// @param memoId ID of Memo.
/// @param tagIds IDs of Tags.
/// @param sqlite3 Wrapper object for sqlite3 db.
/// @return true if insertion succeeded (even if nothing was inserted), false if the operation failed.
bool InsertMemoTagIds(unsigned long memoId, const std::vector<unsigned long>& tagIds, ISqlite3Wrapper& sqlite3);

/// @brief Deletes rows from the 'Tagged' which match the combination of the given Memo ID and Tag IDs. In other words,
///        tags with given IDs are removed from the memo with the provided ID.
/// @param memoId ID of Memo.
/// @param tagIds IDs of Tags.
/// @param sqlite3 Wrapper object for sqlite3 db.
/// @return true if the deletion succeeded (even if nothing was deleted), false if the operation failed.
bool DeleteMemoTagIds(unsigned long memoId, const std::vector<unsigned long>& tagIds, ISqlite3Wrapper& sqlite3);

/// @brief Based on the information in the provided filter, builds a query that returns Tag rows with all of Tag's
///        attributes.
/// @param filter A struct containing criteria based on which tags should be selected.
/// @return A string representing the built SELECT query.
std::string BuildTagQuery(const TagSearchFilter& filter);

} // namespace memo
