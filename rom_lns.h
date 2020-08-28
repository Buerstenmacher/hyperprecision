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
--> we will need an extra step to be able to convert our number back to standard float or double	*/

namespace rom {

template <uint8_t base_digit=16>  // 1 -> base == -1.1;  0 -> base ==-2; 2-> base == -1.01
class lns{
using intt = int64_t;				//type of the underlying integer data
using floatt = rom::floatxx_t<128>;	//floatingpoint type for temporary values

private:
intt _exp;

static floatt _base(void) {	//return the inplizit base of this type
static floatt ret = (floatt{1.0}+ (floatt{10}^((-1)*base_digit))) * -1;
return ret;
}

static floatt _abs_base(void) {	//return the abs of the base of this type
static floatt ret{abs(_base())};
return ret;
}

static floatt _ln_abs_base(void) { //return the ln(abs(_base()))
static floatt ret{log(_abs_base())};
return ret;
}

static floatt _inv_ln_abs_base(void) { //return (1.0/ln(abs(_base())))
static floatt ret{(floatt{1}/_ln_abs_base())};
return ret;
}

int8_t sign(void) const {return (_exp%2)?int8_t(-1):int8_t(1);}

public:
~lns()                              = default;      //default destructor
lns(const lns& in)                  = default;      //default copy should be fast
lns& operator=(const lns& in)       = default;      //default copy assignment
floatt value(void) const 	 		{return  _base()^_exp;}	//get value of number
explicit operator floatt() const 	{return value();}
operator std::string() const 	 	{return std::string(floatt(*this));}			//slow!
lns operator+(const lns& right) const 	{return lns(right.value()+this->value());}	//slow!
lns operator-(const lns& right) const 	{return lns(this->value()-right.value());}	//slow!
lns operator+=(const lns& r) 			{return (*this)=(*this)+r;}				
lns operator-=(const lns& r) 			{return (*this)=(*this)-r;}

lns(const floatt& in):lns() {	//construct from floatt
if (in==0.0) {return;}
int8_t sign__{(in<0)?int8_t{-1}:int8_t{1}};
auto value{abs(in)};
auto close_to_zero{intt(intxx_t(log(value)*_inv_ln_abs_base() ))};	//candidate rounded toward zero
auto farther_from_zero{close_to_zero + sign__};						//candidate rounded away from zero
if (sign__ ==  1) {_exp = (close_to_zero%2)?farther_from_zero:close_to_zero;}
if (sign__ == -1){_exp = (close_to_zero%2)?close_to_zero:farther_from_zero;}
}

lns(void):_exp{std::numeric_limits<intt>::min()} {}	//default construct close to zero

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

lns operator*=(const lns& r) {		//simple fast multiplication
_exp += r._exp;
return *this;
}

lns operator/=(const lns& r) {		//simple fast division
_exp -= r._exp;
return *this;
}

lns operator*(const lns& right) const {
auto th{*this};
return th*=right;
}

lns operator/(const lns& right) const {
auto th{*this};
return th/=right;
}

bool operator< (const lns& r) const {
if (this->sign() != r.sign())	{return (r.sign()==1)?true:false;}
if (r.sign() == 1)				{return this->_exp < r.exp;}
if (r.sign() == -1)				{return this->_exp > r.exp;}
}

bool     operator> (const lns& r) const   {return (r<*this);}
bool     operator==(const lns& r) const   {return this->_exp == r._exp;}
bool     operator!=(const lns& r) const   {return this->_exp != r._exp;}
bool     operator>=(const lns& r) const   {return ((*this>r) or (*this==r));}
bool     operator<=(const lns& r) const   {return ((*this<r) or (*this==r));}

};//class lns

template <uint8_t p>
std::ostream& operator << (std::ostream& os, const lns<p>& v) {
os << std::string(v);
return os;
}

}//namespace rom
#endif //ROM_LNS

