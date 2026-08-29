#pragma once

#include "../../domain/entities/Product.h"

namespace application::dto{
    struct RegisterProdRequest{
        std::string name_;
        double price_;
    };
}