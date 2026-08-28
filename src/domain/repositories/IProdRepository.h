#pragma once

#include <optional>

#include "../entities/Product.h"

namespace domain::repositories{
    class IProdRepository{
        public:
            virtual ~IProdRepository() = default;

            virtual void add(const entities::Product&) = 0;

            virtual std::optional<entities::Product> findByName(const std::string&) = 0;
    };
}