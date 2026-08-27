#pragma once
#include <string>

namespace domain::entities{

class Product{
    public:
        Product(std::string, std::string, double);
        const std::string& id() const noexcept;
        const std::string& name() const noexcept;
        double price() const noexcept;

    private: 
        std::string id_;
        std::string name_;
        double price_;
};
}