// BehaviorAttachableBase.h
#pragma once
#include "IBehaviorAttachable.h"
#include <memory>
#define DECLARE_BEHAVIOR_ATTACHABLE(CLASS, CTOR_ARGS, BODY)                     \
    friend class BehaviorAttachableBase<CLASS>;                                 \
private:                                                                        \
    CLASS CTOR_ARGS : BehaviorAttachableBase<CLASS>() BODY                      \
public:                                                                         \
    template <typename... Args>                                                 \
    static std::shared_ptr<CLASS> create(Args&&... args) {                      \
        auto ptr = std::shared_ptr<CLASS>(                                      \
            new CLASS(std::forward<Args>(args)...)                              \
        );                                                                      \
        ptr->behaviorManager = BehaviorManager<CLASS>(ptr);                     \
        return ptr;                                                             \
    }

template <typename T>
class BehaviorAttachableBase 
    : public IBehaviorAttachable<T>,
      public std::enable_shared_from_this<T> {
protected:
    BehaviorManager<T> behaviorManager;

    BehaviorAttachableBase() : behaviorManager(nullptr) {}

public:
    static std::shared_ptr<T> create() {
        // static_assert to ensure correct CRTP usage
        static_assert(std::is_base_of<BehaviorAttachableBase<T>, T>::value,
                      "Class must inherit from BehaviorAttachableBase<T>");

        auto ptr = std::shared_ptr<T>(new T());
        ptr->behaviorManager = BehaviorManager<T>(ptr);
        return ptr;
    }

    BehaviorManager<T>& getBehaviorManager() override {
        return behaviorManager;
    }

    template <typename B>
    std::shared_ptr<B> getBehavior() {
        return behaviorManager.template getBehavior<B>();
    }
};
