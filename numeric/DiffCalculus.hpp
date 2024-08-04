#pragma once

namespace Differentiator {
    enum FDiffMode {
        Forward = 1, Backward = -1, Central = 0
    };

    template <class TRet, class TArg>
    constexpr TRet FDiff(TRet(*f)(TArg), const TArg x, const TArg h = TArg(1e-6), FDiffMode _mode = Central) {
        switch(_mode) {
            case Forward:
                return (f(x + h) - f(x))/h;
            case Backward:
                return (f(x) - f(x - h))/h;
            case Central:
                return (f(x + h) - f(x - h))/(2.0 * h);
        }
    }
};