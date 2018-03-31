//
//  easing.h
//
//  Copyright (c) 2011, Auerhaus Development, LLC
//
//  This program is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What The Fuck You Want
//  To Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/wtfpl/COPYING for more details.
//

#ifndef DECLARE_EASING
#define DECLARE_EASING(name, func)
#endif

#ifndef GENERATE_EASING_DECLARATIONS
#define GENERATE_EASING_DECLARATIONS \
    DECLARE_EASING(Constantly1, ease::contsant1) \
    DECLARE_EASING(Constantly0, ease::constant0) \
    DECLARE_EASING(Linear, ease::linear) \
    DECLARE_EASING(QuadraticIn, ease::in::quadratic) \
    DECLARE_EASING(QuadraticOut, ease::out::quadratic) \
    DECLARE_EASING(QuadraticInOut, ease::inout::quadratic) \
    DECLARE_EASING(CubicIn, ease::in::cubic) \
    DECLARE_EASING(CubicOut, ease::out::cubic) \
    DECLARE_EASING(CubicInOut, ease::inout::cubic) \
    DECLARE_EASING(QuarticIn, ease::in::quartic) \
    DECLARE_EASING(QuarticOut, ease::out::quartic) \
    DECLARE_EASING(QuarticInOut, ease::inout::quartic) \
    DECLARE_EASING(QuinticIn, ease::in::quintic) \
    DECLARE_EASING(QuinticOut, ease::out::quintic) \
    DECLARE_EASING(QuinticInOut, ease::inout::quintic) \
    DECLARE_EASING(SineIn, ease::in::sine) \
    DECLARE_EASING(SineOut, ease::out::sine) \
    DECLARE_EASING(SineInOut, ease::inout::sine) \
    DECLARE_EASING(CircularIn, ease::in::circular) \
    DECLARE_EASING(CircularOut, ease::out::circular) \
    DECLARE_EASING(CircularInOut, ease::inout::circular) \
    DECLARE_EASING(ExponentialIn, ease::in::exponential) \
    DECLARE_EASING(ExponentialOut, ease::out::exponential) \
    DECLARE_EASING(ExponentialInOut, ease::inout::exponential) \
    DECLARE_EASING(ElasticIn, ease::in::elastic) \
    DECLARE_EASING(ElasticOut, ease::out::elastic) \
    DECLARE_EASING(ElasticInOut, ease::inout::elastic) \
    DECLARE_EASING(BackIn, ease::in::back) \
    DECLARE_EASING(BackOut, ease::out::back) \
    DECLARE_EASING(BounceIn, ease::in::bounce) \
    DECLARE_EASING(BounceOut, ease::out::bounce) \
    DECLARE_EASING(BackInOut, ease::inout::back) \
    DECLARE_EASING(BounceInOut, ease::inout::bounce) \
    DECLARE_EASING(DutyCycle10, ease::dutycycle1) \
    DECLARE_EASING(DutyCycle20, ease::dutycycle2) \
    DECLARE_EASING(DutyCycle30, ease::dutycycle3) \
    DECLARE_EASING(DutyCycle40, ease::dutycycle4) \
    DECLARE_EASING(DutyCycle50, ease::dutycycle5) \
    DECLARE_EASING(DutyCycle60, ease::dutycycle6) \
    DECLARE_EASING(DutyCycle70, ease::dutycycle7) \
    DECLARE_EASING(DutyCycle80, ease::dutycycle8) \
    DECLARE_EASING(DutyCycle90, ease::dutycycle9)

#endif

#ifndef AH_EASING_H
#define AH_EASING_H

#include <functional>
namespace ease {

#if defined(__LP64__) && !defined(AH_EASING_USE_DBL_PRECIS)
#define AH_EASING_USE_DBL_PRECIS
#endif

typedef float AHFloat;

typedef AHFloat (*AHEasingFunction)(AHFloat);
typedef std::function<AHFloat(AHFloat)> Function;

namespace in {
    AHFloat quadratic(AHFloat p); // Quadratic easing; p^2
    AHFloat cubic(AHFloat p); // Cubic easing; p^3
    AHFloat quartic(AHFloat p); // Quartic easing; p^4
    AHFloat quintic(AHFloat p); // Quintic easing; p^5
    AHFloat sine(AHFloat p); // Sine wave easing; sin(p * PI/2)
    AHFloat circular(AHFloat p); // Circular easing; sqrt(1 - p^2)
    AHFloat exponential(AHFloat p); // Exponential easing, base 2
    AHFloat elastic(AHFloat p); // Exponentially-damped sine wave easing
    AHFloat back(AHFloat p); // Overshooting cubic easing
    AHFloat bounce(AHFloat p); // Exponentially-decaying bounce easing
}

namespace out {
    AHFloat quadratic(AHFloat p); // Quadratic easing; p^2
    AHFloat cubic(AHFloat p); // Cubic easing; p^3
    AHFloat quartic(AHFloat p); // Quartic easing; p^4
    AHFloat quintic(AHFloat p); // Quintic easing; p^5
    AHFloat sine(AHFloat p); // Sine wave easing; sin(p * PI/2)
    AHFloat circular(AHFloat p); // Circular easing; sqrt(1 - p^2)
    AHFloat exponential(AHFloat p); // Exponential easing, base 2
    AHFloat elastic(AHFloat p); // Exponentially-damped sine wave easing
    AHFloat back(AHFloat p); // Overshooting cubic easing
    AHFloat bounce(AHFloat p); // Exponentially-decaying bounce easing
}

namespace inout {
    AHFloat quadratic(AHFloat p); // Quadratic easing; p^2
    AHFloat cubic(AHFloat p); // Cubic easing; p^3
    AHFloat quartic(AHFloat p); // Quartic easing; p^4
    AHFloat quintic(AHFloat p); // Quintic easing; p^5
    AHFloat sine(AHFloat p); // Sine wave easing; sin(p * PI/2)
    AHFloat circular(AHFloat p); // Circular easing; sqrt(1 - p^2)
    AHFloat exponential(AHFloat p); // Exponential easing, base 2
    AHFloat elastic(AHFloat p); // Exponentially-damped sine wave easing
    AHFloat back(AHFloat p); // Overshooting cubic easing
    AHFloat bounce(AHFloat p); // Exponentially-decaying bounce easing
}

// Linear interpolation (no easing)
AHFloat linear(AHFloat p);

// Duty-cycle (threshold all-or-nothing)
AHFloat dutycycle1(AHFloat p); // 10%
AHFloat dutycycle2(AHFloat p); // 20%
AHFloat dutycycle3(AHFloat p); // 30%
AHFloat dutycycle4(AHFloat p); // 40%
AHFloat dutycycle5(AHFloat p); // 50%
AHFloat dutycycle6(AHFloat p); // 60%
AHFloat dutycycle7(AHFloat p); // 70%
AHFloat dutycycle8(AHFloat p); // 80%
AHFloat dutycycle9(AHFloat p); // 90%

// Constant
AHFloat constant0(AHFloat p); // always returns 0
AHFloat contsant1(AHFloat p); // always returns 1

} // namespace easing

#endif
