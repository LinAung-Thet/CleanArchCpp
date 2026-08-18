#include <string>

namespace infrastructure::persistence {

template<typename T> class IServerHelper {
public:
    virtual std::string buildInsert(const T& entity) = 0;
};

}