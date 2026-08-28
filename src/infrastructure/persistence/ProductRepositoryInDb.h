#pragma once

#include <optional>

#include "../../domain/entities/Product.h"
#include "../../domain/repositories/IProdRepository.h"
#include "DbAdapter.h"

namespace infrastructure::persistence{
    class ProductRepositoryInDb : domain::repositories::IProdRepository{
        private:
            DbAdapter<domain::entities::Product>& dbAdapter_;
        public:
            ProductRepositoryInDb(DbAdapter<domain::entities::Product>&);

            void add(const domain::entities::Product& product) override;

            std::optional<domain::entities::Product> findByName(const std::string&) override;
    };
}