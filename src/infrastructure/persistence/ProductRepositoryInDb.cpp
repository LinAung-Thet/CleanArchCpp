#pragma once

#include <optional>

#include "ProductRepositoryInDb.h"
#include "ProductSqlTraits.h"
#include "../../domain/entities/Product.h"

namespace infrastructure::persistence{
    using Entity = domain::entities::Product;

    ProductRepositoryInDb::ProductRepositoryInDb(DbAdapter<domain::entities::Product>& dbAdapter) 
    : dbAdapter_(dbAdapter) {}

    void ProductRepositoryInDb::add(const Entity& item){
        dbAdapter_.add(item);
    }

    std::optional<domain::entities::Product> ProductRepositoryInDb::findByName(const std::string& name)
    {
        auto prods = dbAdapter_.findByColumn("Name", name);

        if (!prods.empty())
            return prods[0];
        
        return std::nullopt;
    }
}