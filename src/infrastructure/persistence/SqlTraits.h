#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <Windows.h>
#include <sql.h>
#include <sqlext.h>

namespace infrastructure::persistence
{

template<typename T>
struct SqlTraits;

// ============================================================
// User
// ============================================================

template<>
struct SqlTraits<domain::entities::User>
{
    using Entity = domain::entities::User;

    static constexpr std::string_view table = "Users";

    static constexpr std::array<std::string_view, 3> columns = {
        "Id",
        "Name",
        "Email"
    };

    static constexpr std::array<SQLULEN, 3> columnSizes = {
        50,
        100,
        200
    };

    static constexpr std::size_t columnCount()
    {
        return columns.size();
    }

    // --------------------------------------------------------
    // Convert entity field -> string
    // --------------------------------------------------------

    static std::string getField(
        const Entity& entity,
        std::size_t index)
    {
        switch (index)
        {
        case 0:
            return entity.id();

        case 1:
            return entity.name();

        case 2:
            return entity.email().value();

        default:
            throw std::out_of_range(
                "Invalid User field index");
        }
    }

    // --------------------------------------------------------
    // Construct entity from database row
    // --------------------------------------------------------

    static Entity fromRow(
        const std::vector<std::vector<char>>& buffers)
    {
        if (buffers.size() < columnCount())
        {
            throw std::runtime_error(
                "Insufficient database columns");
        }

        return Entity{
            buffers[0].data(),
            buffers[1].data(),
            domain::value_objects::Email(
                buffers[2].data())
        };
    }
};

} // namespace infrastructure::persistence