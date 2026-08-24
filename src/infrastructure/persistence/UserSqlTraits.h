#pragma once

#include "SqlTraits.h"

#include <stdexcept>
#include <string>

namespace infrastructure::persistence
{
    template<>
    struct SqlTraits<domain::entities::User>
    {
        using Entity = domain::entities::User;

        // --------------------------------------------------------
        // Database metadata
        // --------------------------------------------------------

        // database mapping; How is User represented in the database?
        static SqlMetadata metadata()
        {
            return SqlMetadata{
                "Users",
                {
                    "Id",
                    "Name",
                    "Email"
                }
            };
        }

        // --------------------------------------------------------
        // Entity -> SQL parameters
        // --------------------------------------------------------

        static SqlParameters toParameters(
            const Entity& entity)
        {
            return {
                SqlValue::fromString(
                    entity.id()),

                SqlValue::fromString(
                    entity.name()),

                SqlValue::fromString(
                    entity.email().value())
            };
        }

        // --------------------------------------------------------
        // SQL row -> Entity
        // --------------------------------------------------------

        // object construction; how to construct a User from retrieved data?
        static Entity fromRow(
            const SqlRow& row)
        {
            if (row.size() < columnCount())
            {
                throw std::runtime_error(
                    "Insufficient database columns");
            }

            return Entity{
                row[0],
                row[1],
                domain::value_objects::Email(
                    row[2])
            };
        }

        // --------------------------------------------------------
        // Number of columns
        // --------------------------------------------------------

        static constexpr std::size_t columnCount()
        {
            return 3;
        }
    };
}