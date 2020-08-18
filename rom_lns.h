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

template <uint8_t base_digit=17>  // 1 -> base == -1.1;  0 -> base ==-2; 2-> base == -1.01
class lns{
using intt = int64_t;
using floatt = rom::floatxx_t<128>;

private:
intt _exp;

static floatt _base(void) {	//return the inpizit base of this type
static floatt ret = (floatt{1.0}+ (floatt{10}^((-1)*base_digit))) * -1;
return ret;
}

static floatt _abs_base(void) {	//return the abs of the base of this type
static floatt ret{abs(_base())};
return ret;
}

static floatt _ln_abs_base(void) { //return the ln(abs(_base()))
static floatt ret{floatt::log(_abs_base())};
return ret;
}


public:

static lns _min(void) {
static lns ret{};
ret._exp = std::numeric_limits<intt>::max();
return ret;
}

static lns _max(void) {
static lns ret{};
ret._exp = std::numeric_limits<intt>::max() - 1;
return ret;
}

lns(const floatt& in):lns() {
if (in==0.0) {return;}
int sign{(in<0)?-1:1};
auto value{abs(in)};
auto close_to_zero{intt(intxx_t( floatt::log(value)/_ln_abs_base() ))};	//candidate rounded toward zero
auto farther_from_zero{close_to_zero+sign};				//candidate rounded away from zero
if (sign == 1) {_exp = (close_to_zero%2)?farther_from_zero:close_to_zero;}
if (sign == -1){_exp = (close_to_zero%2)?close_to_zero:farther_from_zero;}
}

lns(void):_exp{std::numeric_limits<intt>::min()} {}	//default construct close to zero

floatt value(void) const {return  _base()^_exp;}	//get value of number
explicit operator floatt() const {return value();}
operator std::string() const 	 {return std::string(floatt(*this));}

};//class lns

template <uint8_t p>
std::ostream& operator << (std::ostream& os, const lns<p>& v) {
os << std::string(v);
return os;
}

}//namespace rom
#endif //ROM_LNS

