
#pragma once
#include "BehaviorManager.h"

template <typename T>
class IBehaviorAttachable {
public:
    virtual ~IBehaviorAttachable() = default;
    virtual BehaviorManager<T>& getBehaviorManager() = 0;

    template <typename B>
    std::shared_ptr<B> getBehavior() {
        return getBehaviorManager().template getBehavior<B>();
    }
};
