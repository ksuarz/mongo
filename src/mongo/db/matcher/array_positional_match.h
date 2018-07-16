/**
 * Copyright (C) 2018 MongoDB Inc.
 *
 * This program is free software: you can redistribute it and/or  modify
 * it under the terms of the GNU Affero General Public License, version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, the copyright holders give permission to link the
 * code of portions of this program with the OpenSSL library under certain
 * conditions as described in each individual source file and distribute
 * linked combinations including the program with the OpenSSL library. You
 * must comply with the GNU Affero General Public License in all respects
 * for all of the code used other than as permitted herein. If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so. If you do not
 * wish to do so, delete this exception statement from your version. If you
 * delete this exception statement from all source files in the program,
 * then also delete it in the license file.
 */

#pragma once

#include <boost/optional.hpp>
#include <memory>
#include <string>

#include "mongo/base/string_data.h"

namespace mongo {
/**
 * Stores the offset into an array for which an element satisfies a match predicate. Used for
 * $elemMatch and $-positional projections.
 */
class ArrayPositionalMatch {
public:
    ArrayPositionalMatch() = default;

    /**
     * Returns true if a request has been made to store the array offset.
     */
    bool arrayPositionRequested() const {
        return _arrayPositionRequested;
    }

    /**
     * The array offset that satisfies the match predicate, if set.
     */
    boost::optional<std::string> arrayPosition() const {
        return _arrayPosition;
    }

    /**
     * Requests that the array match position be recorded.
     */
    void requestArrayPosition();

    /**
     * Sets the array match position to 'pos'. No effect if the array position has not been
     * requested.
     */
    void setArrayPosition(StringData pos);

    /**
     * Resets any recorded array position. (If the position has been requested, though, it remains
     * requested.)
     */
    void reset();

private:
    bool _arrayPositionRequested;

    // If set, this contains the array index for which a match predicate was satisfied. The index is
    // stored as a field name string (e.g. "1").
    boost::optional<std::string> _arrayPosition;
};
}  // namespace mongo
