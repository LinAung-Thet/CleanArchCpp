#include <string>

#include "Product.h"

namespace domain::entities{
    Product::Product(std::string id, std::string name, double price) :
        id_(std::move(id)), name_(std::move(name)), price_(price)
        {}
    const std::string& Product::id() const noexcept{
        return id_;
    }

    const std::string& Product::name() const noexcept{
        return name_;
    }

    double Product::price() const noexcept{
        return price_;
    }
}