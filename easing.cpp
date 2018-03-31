//
//  easing.c
//
//  Copyright (c) 2011, Auerhaus Development, LLC
//
//  This program is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What The Fuck You Want
//  To Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/wtfpl/COPYING for more details.
//

#include <math.h>
#include "easing.h"

namespace ease {

// Modeled after the line y = x
AHFloat linear(AHFloat p)
{
	return p;
}

// Duty-cycle (threshold all-or-nothing)
AHFloat dutycycle1(AHFloat p)
{
    return p > 0.1f ? 1.0f : 0.0f;
}

AHFloat dutycycle2(AHFloat p)
{
    return p > 0.2f ? 1.0f : 0.0f;
}

AHFloat dutycycle3(AHFloat p)
{
    return p > 0.3f ? 1.0f : 0.0f;
}

AHFloat dutycycle4(AHFloat p)
{
    return p > 0.4f ? 1.0f : 0.0f;
}

AHFloat dutycycle5(AHFloat p)
{
    return p > 0.5f ? 1.0f : 0.0f;
}

AHFloat dutycycle6(AHFloat p)
{
    return p > 0.6f ? 1.0f : 0.0f;
}

AHFloat dutycycle7(AHFloat p)
{
    return p > 0.7f ? 1.0f : 0.0f;
}

AHFloat dutycycle8(AHFloat p)
{
    return p > 0.8f ? 1.0f : 0.0f;
}

AHFloat dutycycle9(AHFloat p)
{
    return p > 0.9f ? 1.0f : 0.0f;
}

// Constant
AHFloat constant0(AHFloat)
{
    return 0.0f;
}

AHFloat contsant1(AHFloat)
{
    return 1.0f;
}

// Modeled after the parabola y = x^2
AHFloat in::quadratic(AHFloat p)
{
	return p * p;
}

// Modeled after the parabola y = -x^2 + 2x
AHFloat out::quadratic(AHFloat p)
{
	return -(p * (p - 2));
}

// Modeled after the piecewise quadratic
// y = (1/2)((2x)^2)             ; [0, 0.5)
// y = -(1/2)((2x-1)*(2x-3) - 1) ; [0.5, 1]
AHFloat inout::quadratic(AHFloat p)
{
    if(p < 0.5f)
	{
		return 2 * p * p;
	}
	else
	{
		return (-2 * p * p) + (4 * p) - 1;
	}
}

// Modeled after the cubic y = x^3
AHFloat in::cubic(AHFloat p)
{
	return p * p * p;
}

// Modeled after the cubic y = (x - 1)^3 + 1
AHFloat out::cubic(AHFloat p)
{
	AHFloat f = (p - 1);
	return f * f * f + 1;
}

// Modeled after the piecewise cubic
// y = (1/2)((2x)^3)       ; [0, 0.5)
// y = (1/2)((2x-2)^3 + 2) ; [0.5, 1]
AHFloat inout::cubic(AHFloat p)
{
    if(p < 0.5f)
	{
		return 4 * p * p * p;
	}
	else
	{
		AHFloat f = ((2 * p) - 2);
        return 0.5f * f * f * f + 1;
	}
}

// Modeled after the quartic x^4
AHFloat in::quartic(AHFloat p)
{
	return p * p * p * p;
}

// Modeled after the quartic y = 1 - (x - 1)^4
AHFloat out::quartic(AHFloat p)
{
	AHFloat f = (p - 1);
	return f * f * f * (1 - p) + 1;
}

// Modeled after the piecewise quartic
// y = (1/2)((2x)^4)        ; [0, 0.5)
// y = -(1/2)((2x-2)^4 - 2) ; [0.5, 1]
AHFloat inout::quartic(AHFloat p)
{
    if(p < 0.5f)
	{
		return 8 * p * p * p * p;
	}
	else
	{
		AHFloat f = (p - 1);
		return -8 * f * f * f * f + 1;
	}
}

// Modeled after the quintic y = x^5
AHFloat in::quintic(AHFloat p)
{
	return p * p * p * p * p;
}

// Modeled after the quintic y = (x - 1)^5 + 1
AHFloat out::quintic(AHFloat p)
{
	AHFloat f = (p - 1);
	return f * f * f * f * f + 1;
}

// Modeled after the piecewise quintic
// y = (1/2)((2x)^5)       ; [0, 0.5)
// y = (1/2)((2x-2)^5 + 2) ; [0.5, 1]
AHFloat inout::quintic(AHFloat p)
{
    if(p < 0.5f)
	{
		return 16 * p * p * p * p * p;
	}
	else
	{
		AHFloat f = ((2 * p) - 2);
        return  0.5f * f * f * f * f * f + 1;
	}
}

// Modeled after quarter-cycle of sine wave
AHFloat in::sine(AHFloat p)
{
    return sin((p - 1) * float(M_PI_2)) + 1;
}

// Modeled after quarter-cycle of sine wave (different phase)
AHFloat out::sine(AHFloat p)
{
    return sin(p * float(M_PI_2));
}

// Modeled after half sine wave
AHFloat inout::sine(AHFloat p)
{
    return 0.5f * (1 - cos(p * float(M_PI_2)));
}

// Modeled after shifted quadrant IV of unit circle
AHFloat in::circular(AHFloat p)
{
	return 1 - sqrt(1 - (p * p));
}

// Modeled after shifted quadrant II of unit circle
AHFloat out::circular(AHFloat p)
{
	return sqrt((2 - p) * p);
}

// Modeled after the piecewise circular function
// y = (1/2)(1 - sqrt(1 - 4x^2))           ; [0, 0.5)
// y = (1/2)(sqrt(-(2x - 3)*(2x - 1)) + 1) ; [0.5, 1]
AHFloat inout::circular(AHFloat p)
{
    if(p < 0.5f)
	{
        return 0.5f * (1 - sqrt(1 - 4 * (p * p)));
	}
	else
	{
        return 0.5f * (sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
	}
}

// Modeled after the exponential function y = 2^(10(x - 1))
AHFloat in::exponential(AHFloat p)
{
    return (p == 0.0f) ? p : pow(2, 10 * (p - 1));
}

// Modeled after the exponential function y = -2^(-10x) + 1
AHFloat out::exponential(AHFloat p)
{
    return (p == 1.0f) ? p : 1 - pow(2, -10 * p);
}

// Modeled after the piecewise exponential
// y = (1/2)2^(10(2x - 1))         ; [0,0.5)
// y = -(1/2)*2^(-10(2x - 1))) + 1 ; [0.5,1]
AHFloat inout::exponential(AHFloat p)
{
    if(p == 0.0f || p == 1.0f) return p;
	
    if(p < 0.5f)
	{
        return 0.5f * float(pow(2, (20 * p) - 10));
	}
	else
	{
        return -0.5f * float(pow(2, (-20 * p) + 10)) + 1;
	}
}

// Modeled after the damped sine wave y = sin(13pi/2*x)*pow(2, 10 * (x - 1))
AHFloat in::elastic(AHFloat p)
{
    return sin(13 * float(M_PI_2) * p) * float(pow(2, 10 * (p - 1)));
}

// Modeled after the damped sine wave y = sin(-13pi/2*(x + 1))*pow(2, -10x) + 1
AHFloat out::elastic(AHFloat p)
{
    return sin(-13 * float(M_PI_2) * (p + 1)) * float(pow(2, -10 * p)) + 1;
}

// Modeled after the piecewise exponentially-damped sine wave:
// y = (1/2)*sin(13pi/2*(2*x))*pow(2, 10 * ((2*x) - 1))      ; [0,0.5)
// y = (1/2)*(sin(-13pi/2*((2x-1)+1))*pow(2,-10(2*x-1)) + 2) ; [0.5, 1]
AHFloat inout::elastic(AHFloat p)
{
    if(p < 0.5f)
	{
        return 0.5f * sin(13 * float(M_PI) * (2 * p)) * float(pow(2, 10 * ((2 * p) - 1)));
	}
	else
	{
        return 0.5f * (sin(-13 * float(M_PI) * ((2 * p - 1) + 1)) * float(pow(2, -10 * (2 * p - 1))) + 2);
	}
}

// Modeled after the overshooting cubic y = x^3-x*sin(x*pi)
AHFloat in::back(AHFloat p)
{
    return p * p * p - p * sin(p * float(M_PI));
}

// Modeled after overshooting cubic y = 1-((1-x)^3-(1-x)*sin((1-x)*pi))
AHFloat out::back(AHFloat p)
{
	AHFloat f = (1 - p);
    return 1 - (f * f * f - f * sin(f * float(M_PI)));
}

// Modeled after the piecewise overshooting cubic function:
// y = (1/2)*((2x)^3-(2x)*sin(2*x*pi))           ; [0, 0.5)
// y = (1/2)*(1-((1-x)^3-(1-x)*sin((1-x)*pi))+1) ; [0.5, 1]
AHFloat inout::back(AHFloat p)
{
    if(p < 0.5f)
	{
		AHFloat f = 2 * p;
        return 0.5f * (f * f * f - f * sin(f * float(M_PI)));
	}
	else
	{
		AHFloat f = (1 - (2*p - 1));
        return 0.5f * (1 - (f * f * f - f * sin(f * float(M_PI)))) + 0.5f;
	}
}

AHFloat in::bounce(AHFloat p)
{
    return 1 - out::bounce(1 - p);
}

AHFloat out::bounce(AHFloat p)
{
    if(p < 4/11.0f)
	{
        return (121 * p * p)/16.0f;
	}
    else if(p < 8/11.0f)
	{
        return (363/40.0f * p * p) - (99/10.0f * p) + 17/5.0f;
	}
    else if(p < 9/10.0f)
	{
        return (4356/361.0f * p * p) - (35442/1805.0f * p) + 16061/1805.0f;
	}
	else
	{
        return (54/5.0f * p * p) - (513/25.0f * p) + 268/25.0f;
	}
}

AHFloat inout::bounce(AHFloat p)
{
    if(p < 0.5f)
	{
        return 0.5f * in::bounce(p*2);
	}
	else
	{
        return 0.5f * out::bounce(p * 2 - 1) + 0.5f;
	}
}

} // namespace ease
