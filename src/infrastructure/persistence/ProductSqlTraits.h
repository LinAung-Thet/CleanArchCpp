#pragma once

#include <stdexcept>

// #include "../../domain/entities/Product.h"
#include "SqlTraits.h"
#include "../../domain/entities/Product.h"

namespace infrastructure::persistence {

    template<>
    struct SqlTraits<domain::entities::Product>{
        using Entity = domain::entities::Product;
        // return table name and column names
        static SqlMetadata metadata()
        {
            return SqlMetadata{
                "Products",
                {"Id", "Name", "Price"}
            };
        }

        static SqlParameters toParameters(const Entity& entity){
            return {
                SqlValue::fromString(entity.id()),
                SqlValue::fromString(entity.name()),
                SqlValue::fromString(std::to_string(entity.price()))
            };
        }

        static Entity toEntity(const SqlRow& row){
            if (row.size() < columnCount()){
                throw std::runtime_error(
                    "Insufficient database columns!"
                );
            }

            return Entity(
                row[0],
                row[1],
                std::stod(row[2])
            );
        }

        static constexpr int columnCount(){
            return 3;
        }
    };
}