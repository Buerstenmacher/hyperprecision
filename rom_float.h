#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <climits>
#include <limits>
#include <type_traits>

#ifndef ROM_FLOAT
#define ROM_FLOAT

#include "rom_int.h"
#include "rom_math.h"

namespace rom {

////////////////////////////////////////////////////////////////////////////////////////////////////
template <size_t precision = 64>     //intended numer of bits representing the mantissa
class floatxx_t {
private:        //value == _mant * 2.0^_exp
using temp_type = floatxx_t<precision + 16>;	//some temporary values will be stored with extra precision
intxx_t _exp;    //exponent to the base 2.0
intxx_t _mant;   //mantissa;

void shift_mantissa_right(int64_t digits) {     //if digits is negative shift left
//if      (digits>0)      {_mant >>= digits;}		//left shift with negative input will always preserve value and precision
if      (digits>0)      {_mant.shift_right_and_round(digits);}	//left shift with negative input will always preserve value and precision
else if (digits<0)      {_mant <<= std::abs(digits);}	//right shift with positive input should preserve value but precision might be lost
_exp += digits;
}

//inflate of collapse mantissa to <precision> size
void renormalize(size_t ndig = precision) {
shift_mantissa_right(int64_t(_mant.effective_size())-int64_t(ndig));
}

//inflate or deflate mantissa that exponent becomes zero
void make_exponent_zero(void)   {shift_mantissa_right(-1*int64_t(_exp));}

//match both parameters to the highest precision of the two
static void match_exponent_to_highest_precission(floatxx_t& l, floatxx_t& r) {
if (l._mant == 0) {l._exp = r._exp;}//if mantissa is zero you can make the exponent everything you want it to be
if (r._mant == 0) {r._exp = l._exp;}
if      (l._exp > r._exp) {l.shift_mantissa_right(int64_t(r._exp-l._exp));}
else if (l._exp < r._exp) {r.shift_mantissa_right(int64_t(l._exp-r._exp));}
}

//match both parameters to the lowest precision of the two
static void match_exponent_to_lowest_precission(floatxx_t& l, floatxx_t& r) {
if (l._mant == 0) {l._exp = r._exp;}//if mantissa is zero you can make the exponent everything you want it to be
if (r._mant == 0) {r._exp = l._exp;}
if      (l._exp > r._exp) {r.shift_mantissa_right(int64_t(l._exp-r._exp));}
else if (l._exp < r._exp) {l.shift_mantissa_right(int64_t(r._exp-l._exp));}
}

static floatxx_t convert_from_double(double in) {
static floatxx_t one    {intxx_t(int(1))};
static floatxx_t two    {intxx_t(int(2))};
static floatxx_t neg_one{one-two};
static floatxx_t half   {one /  two};
if (in == 0.0) 					{return one-one;}//todo: find out why floatxx_t{} does not behave like floatxx_t{int(1)}-floatxx_t{int(1)}
if (in<0)                                       {return neg_one * convert_from_double(-1.0* in);}
if (in>std::numeric_limits<int64_t>::max())     {return two     * convert_from_double(0.5 * in);}
if (in!=double(int64_t(in)))                    {return half    * convert_from_double(in * 2.0);}
return intxx_t(int64_t(in));
}

intxx_t dezimal_exponent(void) const {//107.56 > 2, 12.0 > 1, 7098678,3 > 6, 0.7 > -1
intxx_t intpart;
temp_type th{abs(*this)};
return intxx_t( (modf(temp_type::log(th,10),&intpart)<0.0)?(intpart-1):(intpart));
}

constexpr size_t dezimal_precision(void)  const {return precision/4;}     //intended numer of digits of

static floatxx_t log_core(floatxx_t x) {	//Returns the natural log of x; abs(xin) should be smaller than 2.0
x -= 1;                  			//this value has to be smaller than 1.0 otherwise our
floatxx_t val{0.0};				//taylor series will never converge --> infinite loop
floatxx_t last_val{0.0};
size_t i{1};
do      {
        last_val = val;
        val += (x^i)/floatxx_t(i);
        ++i;
        if (val == last_val) {break;}
        last_val = val;
        val -= (x^i)/floatxx_t(i);
        ++i;
        } while (last_val != val);
return val;
}

static floatxx_t log_core_2(const floatxx_t& xin) {	//Returns the natural logarithm of x
static const auto flt1_9{floatxx_t{1.9}};		//should work for all values that are larger than 0.0
if (xin<=0)     {throw std::runtime_error("cannot calculate ln of negative number");}
if (xin > flt1_9) {     //shotcut for large numbers
        static auto log_1_9{log_core(flt1_9)};
        return  log_1_9 + log_core_2(xin/flt1_9);
        }
auto ret = floatxx_t::log_core(xin);
return ret;
}

static floatxx_t exp_core(const floatxx_t& inp) {	//only for use with exp() function!!!!!!!
floatxx_t val{0};				//we are using taylor series to calculate this value
floatxx_t last_val{0};
size_t i{0};
do      {
        last_val = val;
        val += (inp^i)*factorial_inv<floatxx_t>(i);
        ++i;
        } while (last_val != val);
return val;
}

static floatxx_t _2pow(const intxx_t& x) {	//this should calculate 2^x
static floatxx_t ret;
ret._mant = intxx_t{1};
ret._exp  = x;
return ret;
}

static floatxx_t _2pow(const floatxx_t& x) {	//this should calculate 2^x //overload for floating point exponent
static floatxx_t ln2{log_core_2(2)};
intxx_t intp;
floatxx_t fracp{modf(x,&intp)};
return _2pow(intp)*exp_core(ln2*fracp);
}

public:
floatxx_t(const intxx_t& in):_exp{0},_mant{in} {renormalize();}
~floatxx_t()                                    = default;      //default destructor
floatxx_t(const floatxx_t& in)                  = default;      //default copy
floatxx_t& operator=(const floatxx_t& in)       = default;      //default copy assignment
floatxx_t(void):_exp{0},_mant{0} {}		//0.0 default
floatxx_t(double in):_exp{0},_mant{0} {(*this) = convert_from_double(in);}

template <size_t dig>     //intended numer of bits representing the mantissa
friend class floatxx_t;
template <size_t dig>     //intended numer of bits representing the mantissa
floatxx_t(const floatxx_t<dig>& in):_exp{in._exp},_mant{in._mant} {renormalize();}//conversion from this type with different template parameters

static floatxx_t exp(const floatxx_t& xin) {	//Returns e^x
static auto ln2{log_core_2(2)};
auto ret {_2pow(xin/ln2)};
return ret;
}

static floatxx_t log(const floatxx_t& xin) {	//Returns the natural logarithm of x
floatxx_t m{xin._mant};			//split in mantissa and exponent
floatxx_t e{xin._exp};
static auto log_2{log_core_2(2)}; //ln(2.0)
auto ret{e*log_2 + log_core_2(m)};
return ret;
}

static floatxx_t log(const floatxx_t& x,const floatxx_t& bas) {return log(x)/log(bas);}//overload for arbitrary base

explicit operator intxx_t() const {
auto th{*this};
th.make_exponent_zero();
return th._mant;
}

operator std::string() const {  //dezimal output in scientific notatation
intxx_t ep10{dezimal_exponent()};
auto ten_pow = floatxx_t{10.0} ^ (intxx_t(dezimal_precision())-ep10) ;
auto tint = intxx_t((*this)*ten_pow);
auto d = std::string(tint);
d.insert(d.size()-dezimal_precision(),".");
return d+ "e" + std::string(ep10);
}

floatxx_t operator*=(const floatxx_t& r) {
static floatxx_t zero{intxx_t(int(0))};
if (r._mant==0)		{return zero;}
if (this->_mant==0)	{return zero;}
auto& th{*this};
th._exp += r._exp;
th._mant *= r._mant;
th.renormalize();
return th;
}

floatxx_t operator/=(const floatxx_t& r) {
auto& th{*this};
if (r._mant==0)	{throw std::runtime_error("division by zero in rom::floatxx_t");}
th.renormalize(2*precision);    //double the size of mantissa to get best acuracy for division
th._exp -= r._exp;
th._mant /= r._mant;
th.renormalize();
return th;
}

floatxx_t operator+=(floatxx_t r) {	//todo: there is a problem if exp and mant is 0
auto& th{*this};			//find it!
match_exponent_to_lowest_precission(th,r);
th._mant+=r._mant;
th.renormalize();
return th;
}

floatxx_t operator-=(floatxx_t r) {
auto& th{*this};
match_exponent_to_lowest_precission(th,r);
th._mant-=r._mant;
th.renormalize();
return th;
}

bool     operator< (floatxx_t r) const {
auto th{*this}; //copy
match_exponent_to_highest_precission(th,r);
return (th._mant < r._mant);
}

bool     operator!=(const floatxx_t& r) const   {return ((*this<r) || (*this>r));}
bool     operator==(const floatxx_t& r) const   {return !(*this!=r);}
bool     operator> (const floatxx_t& r) const   {return (r<*this);}
bool     operator>=(const floatxx_t& r) const   {return ((*this>r) or (*this==r));}
bool     operator<=(const floatxx_t& r) const   {return ((*this<r) or (*this==r));}
floatxx_t operator^=(int64_t r)                 {return (*this)=(*this)^r;}
floatxx_t operator^=(intxx_t r)                 {return (*this)=(*this)^r;}

floatxx_t operator*(const floatxx_t& right) const {
auto th{*this};
return th*=right;
}

floatxx_t operator/(const floatxx_t& right) const {
auto th{*this};
return th/=right;
}

floatxx_t operator+(const floatxx_t& right) const {
auto th{*this};
return th+=right;
}

floatxx_t operator-(const floatxx_t& right) const {
auto th{*this};
return th-=right;
}

floatxx_t operator^(int64_t r) const {return (*this)^intxx_t{r};}

floatxx_t operator^(const intxx_t& r) const {//exponentiation by integer
static floatxx_t zero{intxx_t(int(0))};
static floatxx_t one {intxx_t(int(1))};
if (r     == 0) 	{return one;}
if (*this == zero) 	{return zero;}
if (r == 1)		{return (*this);}
if (rom::abs(r) > 32) {//if exponent is larger than 32 we will break it down to two operation
	if (r%2) {//odd
		if (r>0) {return (((*this)^(r/2))^2) * (*this); }
		if (r<0) {return (((*this)^(r/2))^2) / (*this); }
		}
	else 	 {return (((*this)^(r/2))^2);	 	} 	//even
	}
floatxx_t tmp{one};
if (r > 0)      {for (intxx_t n{0};n!=r;++n)    {tmp *= (*this);}}
if (r < 0)      {for (intxx_t n{0};n!=r;--n)    {tmp *= (*this);}
		tmp = one/tmp;
		}
tmp.renormalize();
return tmp;
}

};	//class floatxx_t

}	//namespace rom
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//////OUTPUT OPERATORS /////////////////////////////////////////////////////////////////////////////

template <size_t p>
std::ostream& operator << (std::ostream& os, const rom::floatxx_t<p>& v) {
os << std::string(v);
return os;
}

#endif	//ROM_FLOAT












