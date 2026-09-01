#pragma once

#include "../../domain/repositories/IProdRepository.h"
#include "../dto/RegisterProdRequest.h"

namespace application::use_cases{
    class RegisterProd{
        private:
            domain::repositories::IProdRepository& dbRepo_;

            int generateProdId();

        public:
        explicit RegisterProd(domain::repositories::IProdRepository&);

        std::string execute(const dto::RegisterProdRequest&);

    };
}