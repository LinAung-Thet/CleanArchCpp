#pragma once

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace infrastructure::persistence
{
    // ------------------------------------------------------------
    // Database-neutral SQL value
    // ------------------------------------------------------------

    struct SqlValue
    {
        std::string value;
        bool isNull{false};

        static SqlValue null()
        {
            return SqlValue{
                {},
                true
            };
        }

        static SqlValue fromString(
            std::string value)
        {
            return SqlValue{
                std::move(value),
                false
            };
        }
    };

    using SqlParameters =
        std::vector<SqlValue>;

    // ------------------------------------------------------------
    // Database-neutral row
    // ------------------------------------------------------------

    using SqlRow =
        std::vector<std::string>;

    using SqlRows =
        std::vector<SqlRow>;

    // ------------------------------------------------------------
    // Entity SQL metadata
    // ------------------------------------------------------------

    struct SqlMetadata
    {
        std::string tableName;
        std::vector<std::string> columns;
    };

    // ------------------------------------------------------------
    // Generic SqlTraits
    //
    // There is intentionally no implementation here.
    // Each persistent entity provides a specialization.
    // ------------------------------------------------------------

    template<typename T>
    struct SqlTraits;
}