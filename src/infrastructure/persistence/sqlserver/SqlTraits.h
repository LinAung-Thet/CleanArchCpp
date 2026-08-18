#pragma once

#include <array>
#include <string>
#include <stdexcept>
#include <Windows.h>
#include <sqltypes.h>
#include "../../../domain/entities/User.h"

namespace infrastructure::persistence::sqlserver {

template <typename T> struct SqlTraits;

template <> struct SqlTraits<domain::entities::User> {
    static constexpr const char* table = "Users";

    static constexpr std::array<const char*, 3> columns = {
        "Id", "Name", "Email"
    };

    static constexpr std::array<SQLULEN, 3> columnSizes = {
        50, 100, 200
    };

    static std::string getField(const domain::entities::User& u, size_t index) {
        switch (index) {
            case 0: return u.id();
            case 1: return u.name();
            case 2: return u.email().value();
        }
        throw std::out_of_range("Invalid field index");
    }

    // NEW: Construct entity from SQL buffers
    static domain::entities::User fromRow(
        const char* id,
        const char* name,
        const char* email
    ) {
        return domain::entities::User{
            id,
            name,
            domain::value_objects::Email(email)
        };
    }
};
}