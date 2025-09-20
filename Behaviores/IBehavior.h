#pragma once
#include <memory>

template<typename T>
class IBehavior {
public:
    virtual ~IBehavior() = default;
    virtual void attach(std::shared_ptr<T> owner) = 0;
};