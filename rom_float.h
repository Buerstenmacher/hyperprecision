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
template <size_t precision>     //intended numer of bits representing the mantissa
class floatxx_t {
private:        //value == _mant * 2.0^_exp
using temp_type = floatxx_t<precision + 32>;	//some temporary values will be stored with extra precision
intxx_t _exp;    //exponent to the base 2.0
intxx_t _mant;   //mantissa;

template <size_t p>
friend auto log(const floatxx_t<p>& xin) -> floatxx_t<p>;

template <size_t p>
friend auto exp(const floatxx_t<p>& xin) -> floatxx_t<p>;

template <size_t p>
friend floatxx_t<p> log(const floatxx_t<p>& x,const floatxx_t<p>& bas);//overload for arbitrary base

void shift_mantissa_right(int64_t digits) {     //if digits is negative shift left //round or cut of if right shift
if      (digits>0) 	{_mant >>= digits;}
else if (digits<0)      {_mant <<= std::abs(digits);}		//left shift with negative input will always preserve value and precision	
_exp += digits;
}

//inflate of collapse mantissa to <precision> size
void renormalize(size_t ndig = precision) {shift_mantissa_right(int64_t(intxx_t(_mant.effective_size())-intxx_t(ndig)));} 

//inflate or deflate mantissa that exponent becomes zero
void make_exponent_zero(void)   {shift_mantissa_right(int64_t(intxx_t(-1)*intxx_t(_exp)));}

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
intxx_t intpart;				//todo:  number is larger than 10^1000000000 there arises a problem because of inaccuracy of floatxx_t
floatxx_t th{abs(*this)};			//for googolplex it returns 10000000000000038749653199388290485261558442988368952743803511717021346139106057529486826921158246400
return intxx_t( (modf(log(th,floatxx_t{10}),&intpart)<0.0)?(intpart-1):(intpart));
}

constexpr size_t dezimal_precision(void)  const {return precision/4;}     //intended numer of digits of

static floatxx_t _2pow(const intxx_t& x) {	//this should calculate 2^x
static floatxx_t ret;
ret._mant = intxx_t{1};
ret._exp  = x;
return ret;
}

static floatxx_t _2pow(const floatxx_t& x) {	//this should calculate 2^x //overload for floating point exponent
static floatxx_t ln2{log(floatxx_t{2})};
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

//conversion from this type with different template parameters
template <size_t dig>
friend class floatxx_t;
template <size_t dig>
explicit floatxx_t(const floatxx_t<dig>& in):_exp{in._exp},_mant{in._mant} {renormalize();}

explicit operator intxx_t() const {
auto th{*this};
th.make_exponent_zero();
return th._mant;
}

operator std::string() const {  //dezimal output in scientific notatation
intxx_t ep10{dezimal_exponent()};
auto ten_pow = floatxx_t{10.0} ^ (intxx_t(dezimal_precision())-ep10) ;
auto tmp{(*this)*ten_pow};
auto tint = intxx_t(tmp);
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
bool     operator>=(const floatxx_t& r) const   {return ((*this>r) or (*this==r));}//todo this could be easier
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
if (r == 2)		{return ((*this) * (*this));}
if (rom::abs(r) > 8) {//if exponent is larger than 8 we will break it down to two operation
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

