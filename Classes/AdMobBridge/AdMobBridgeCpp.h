#ifndef ADMOB_BRIDGE_CPP_H
#define ADMOB_BRIDGE_CPP_H

#include <functional>
#include <string>
#include "cocos2d.h"

namespace admob {

inline void dispatchOnCocosThread(std::function<void()> fn) {
    cocos2d::Director::getInstance()->getScheduler()
        ->performFunctionInCocosThread(std::move(fn));
}

}  // namespace admob

#endif
