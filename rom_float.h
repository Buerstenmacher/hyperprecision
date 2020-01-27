#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <climits>
#include <limits>
#include <type_traits>
#include "rom_int.h"

#ifndef ROM_FLOAT
#define ROM_FLOAT

namespace rom {

template <size_t precision = 16>
class floatxx_t;
/*std::ostream& operator << (std::ostream& os, const rom::floatxx_t<>& v);
*/
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
template <size_t precision>     //intended numer of bits representing the mantissa
class floatxx_t {
private:        //value == mant * 2.0^exp
intxx_t exp;    //exponent to the base 2.0
intxx_t mant;   //mantissa;

void shift_mantissa_right(int64_t digits) {     //if digits is negative shift left
if      (digits>0)      {mant >>= digits;}              //left shift with negative input will always preserve value and precision
else if (digits<0)      {mant <<= std::abs(digits);}    //right shift with positive input should preserve value but precision might be lost
exp += digits;
}

//inflate of collapse mantissa to <precision> size
void renormalize(size_t ndig = precision) {shift_mantissa_right(mant.effective_size()-ndig);}

//inflate or deflate mantiss that exponent becomes zero
void make_exponent_zero(void)   {shift_mantissa_right(-1*int64_t(exp));}

//match both parameters to the highest precision of the two
static void match_exponent_to_highest_precission(floatxx_t& l, floatxx_t& r) {
if      (l.exp > r.exp) {l.shift_mantissa_right(int64_t(r.exp-l.exp));}
else if (l.exp < r.exp) {r.shift_mantissa_right(int64_t(l.exp-r.exp));}
}

//match both parameters to the lowest precision of the two
static void match_exponent_to_lowest_precission(floatxx_t& l, floatxx_t& r) {
if      (l.exp > r.exp) {r.shift_mantissa_right(int64_t(l.exp-r.exp));}
else if (l.exp < r.exp) {l.shift_mantissa_right(int64_t(r.exp-l.exp));}
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

intxx_t dezimal_exponent(void) const {
auto th{*this};
if ((*this)==0) {return 1;} 
th *= (th<0.0)?(-1.0):(1.0);
static floatxx_t ten{10.0};
static floatxx_t tenth{0.1};
static floatxx_t one{1.0};
int64_t ret{0};
while (th < one) {
        th *= ten;
        ret--;
        }
while (th >= ten) {
        th*=tenth;
        ret++;
        }
return ret;
}

size_t dezimal_precision(void)  const {return precision/4;}     //intended numer of digits of

public:
floatxx_t(const intxx_t& in):exp{0},mant{in} {renormalize();}
~floatxx_t()                                    = default;      //default destructor
floatxx_t(const floatxx_t& in)                  = default;      //default copy
floatxx_t& operator=(const floatxx_t& in)       = default;      //default copy assignment
floatxx_t(void):exp{0},mant{0} {}				//0.0 default
floatxx_t(double in):exp{0},mant{0} {(*this) = convert_from_double(in);}

explicit operator intxx_t() const {
auto th{*this};
th.make_exponent_zero();
return th.mant;
}

/*
operator std::string() const { //output with exponent power of 2
auto th{*this};
th.renormalize();
return std::string(th.mant)+" *2^ "+std::string(th.exp);
}*/

operator std::string() const {  //dezimal output in scientific notatation
auto ep10{dezimal_exponent()};
floatxx_t ten_pow = floatxx_t{10.0} ^ (ep10-dezimal_precision()+1) ;
auto d = std::string(intxx_t((*this)/ten_pow));
d.insert(2,".");
return d+ "e" + std::string(ep10);
}

floatxx_t operator*=(const floatxx_t& r) {
auto& th{*this};
th.exp += r.exp;
th.mant *= r.mant;
th.renormalize();
return th;
}

floatxx_t operator/=(const floatxx_t& r) {
auto& th{*this};
th.renormalize(2*precision);    //double the size of mantissa to get best acuracy for division
th.exp -= r.exp;
th.mant /= r.mant;
th.renormalize();
return th;
}

floatxx_t operator+=(floatxx_t r) {
auto& th{*this};
match_exponent_to_lowest_precission(th,r);
th.mant+=r.mant;
th.renormalize();
return th;
}

floatxx_t operator-=(floatxx_t r) {
auto& th{*this};
match_exponent_to_lowest_precission(th,r);
th.mant-=r.mant;
th.renormalize();
return th;
}

bool     operator< (floatxx_t r) const {
auto th{*this}; //copy
match_exponent_to_highest_precission(th,r);
return (th.mant < r.mant);
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

floatxx_t operator^(int64_t r) const {          //exponentiation
floatxx_t ret{1.0};
if (r > 0)      {for (int64_t n{0};n!=r;++n)    {ret*=(*this);}}
if (r < 0)      {
	auto inverse{floatxx_t{1.0}/(*this)};
	for (int64_t n{0};n!=r;--n)    {ret*=inverse;}
	}
return ret;
}

floatxx_t operator^(const intxx_t& r) const {           //exponentiation
floatxx_t ret{1.0};
if (r > 0)      {for (intxx_t n{0};n!=r;++n)    {ret*=(*this);}}
if (r < 0)      {for (intxx_t n{0};n!=r;--n)    {ret/=(*this);}}
return ret;
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

#endif












