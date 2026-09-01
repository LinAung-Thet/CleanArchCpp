#pragma once

#include "RegisterProd.h"
#include "../../domain/entities/Product.h"

namespace application::use_cases{
    RegisterProd::RegisterProd(domain::repositories::IProdRepository& dbRepo) :
    dbRepo_(dbRepo) {}

    std::string RegisterProd::execute(const dto::RegisterProdRequest& req){
        auto existing = dbRepo_.findByName(req.name_);
        if (existing.has_value())
            return "Product currently exists with the name: " + req.name_;
            
        domain::entities::Product prod(std::to_string(generateProdId()), req.name_, req.price_);
        dbRepo_.add(prod);

        return "Product registerred with the name: " + req.name_;
    }
}