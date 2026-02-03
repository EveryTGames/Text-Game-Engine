#pragma once
#include <memory>

template<typename T>
class IBehavior {
public:
    virtual ~IBehavior() = default;
    //T here is what the behaviour is allowed to be added onto
    virtual void attach(std::shared_ptr<T> owner) = 0;
};