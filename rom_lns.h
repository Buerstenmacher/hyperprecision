#include "rom_float.h"
#include "rom_math.h"
#include <limits>

#ifndef ROM_LNS
#define ROM_LNS

//UNFINISHED DO NOT USE THIS FILE!!!!!!!!!!
/*
the purpose of this file is to implement an logarithmic number type in c++11.
it should be able to store numeric values similar to floating point variables and should be able to
do calculations.
every value should be represented by
VALUE == (BASE ^ EXPONENT) * SIGN
BASE 	should be a compiletime constant wich does not need any ram to be remembered
	it should be an number that is slightly larger than 1.0, perhaps 1.000000001 or similar
EXPONENT should be a built in signed integer
SIGN 	should only use one bit of ram
to make calculations as fast as possible, we define SIGN to be -1 if (EXPONENT%2 !=0) and to be 1 if (EXPONENT%2 ==0)

compared to standard floatingpoint nubers we will expect to see some diferences:
pro:
-->multiplication and division is fast
-->precision should be higher withou using more bits per variable
-->exponentiation should be verry fast

contra:
-->addition and subtraction is slow
-->we can not represent exact integer nubers by this type
--> we will need an extra step to be able to convert our number back to standard float or double
*/
namespace rom {

template <uint8_t base_digit=15>  // 1 -> base == 1.1;  0 -> base ==2; 2-> base == 1.01
class lns{
using intt = int64_t;
using floatt = rom::floatxx_t<128>;

private:
intt _exp;

static floatt _base(void) {
static floatt ret = floatt{1.0}+ (floatt{10}^(0-base_digit));
return ret;
}

public:
lns(void):_exp{std::numeric_limits<intt>::min()} {}

floatt value(void) {
auto ret = _base()^_exp;
ret *= (_exp%2)?(-1):(1) ;
return ret;
}


};//class lns
}//namespace rom
#endif //ROM_LNS

