/* this file declares and defines two unlimited dynamic integer types
1.	rom::uintxx_t 	an unsigned integer with dynamic and possibly unlimited size
2. 	rom::intxx_t	an igned integer with dynamic and possibly unlimited size
recomendation:  only use rom::intxx_t type to be safe if negative values come up	*/
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <climits>
#include <limits>

namespace rom {

class uintxx_t;							//there will be a class uintxx_t soon
std::ostream& operator << (std::ostream& os, const ::rom::uintxx_t& v);//there will be an output operator

inline uint8_t getbit(uint64_t in, uint8_t nthbit) {//get the value of the nth bit out of one uint64_t
if (nthbit>63) {return 0;}
static uint64_t mask;
mask = uint64_t(1) << nthbit;
return ((mask & in) >> nthbit);
}

template <class ui> 		//any type of integer
ui factorial(size_t inp) {	//calculate inp!
ui res{1};
for (;inp!=0;inp--) {res *= inp;}
return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
class uintxx_t {        //this class should create an unlimitided unsigned integer type
private:		//we will only declare one member variable
std::vector<bool> _d;	//the bit at(n) has a rank of 2^n; there is only 1 data member

size_t size(void) const	{	//get the size in bytes of this number
size_t s{_d.size()};
if (!s) {return 0;}
do 	{if(this->at(--s)) {return(s+1);}} while (s);	//ignore leading zeros
return (0);
}

void reserve(size_t n) {_d.reserve(n+32);}	//32 bits are extra cushion

bool at(size_t nr) const{		//access individul bit
return (nr<_d.size())?_d.at(nr):false;	//get false==zero if you access bits out of range
}

//bool& at(size_t nr) 	{return _d.at(nr);}	//sorry but non-const-.at() does not work on std::vector<bool>

void trim(void) {_d.resize(size(),false);}	//remove all zeroes from _d that are meaningless

size_t decimal_size(void) {	//get the size of the dezimal representation of this number
if ((*this)==0) {return 1;}	//the value of zero is the only one with a leading zero (false)
uintxx_t compare{1};
size_t digits{0};
do 	{
	digits++;
	compare *= 10;
	} while(compare <= *this);
return digits;
}

public:
uintxx_t(void):_d{} {};			//default constructor; empty _d means value is zero
~uintxx_t() = default;			//default destructor
uintxx_t(const uintxx_t& in) = default;	//default copy
uintxx_t& operator=(const uintxx_t& in) = default;//default copy assignment

uintxx_t(uint64_t inp):_d{} {		//construct from built in uint64_t
for (uint8_t i{0};i!=64;++i) {_d.push_back(getbit(inp,i));}
trim();
}

explicit operator uint64_t() const{	//convert it back to uint64_t
if ((*this).size()>64)	{
	throw std::runtime_error ("a number of type uintxx_t is to large to be converted to uint64_t");
	}
uint64_t res{0};
size_t i{_d.size()};
while (i-- != 0) {
	res <<= 1;
	res += _d.at(i);
	}
return res;
}

explicit operator unsigned char() const{	//convert it back to uint8_t
uint64_t uchar_n_values{UCHAR_MAX+1};
return uint64_t((*this)%uchar_n_values);
}

/*
operator std::string() const{		//convert to binary std::string
std::string ret{};
for (auto it{_d.rbegin()};it!=_d.rend();++it) {ret += (*it)?'1':'0';}
ret += " "+std::to_string(_d.size())+"bits";
return ret;
}
*/

operator std::string() const{		//convert to decimal std::string
if ((*this)==0) {return "0";}
std::string ret{};	//return value
uintxx_t th = {*this};	//copy
std::vector<uintxx_t> ten_power{};
uintxx_t p{1};
size_t decs{th.decimal_size()};
for (size_t sz{0};sz!=decs;++sz) {
	ten_power.push_back(p);	//{1,10,100,1000,10000,....}
	p*=10;
	}
for (auto sz{decs};sz!=0;sz--) {
	const auto& divisor{ten_power.at(sz-1)};
	auto digit{th/divisor};
	if ((digit <= 9)&&(digit>=0))	{ret += '0'+ (unsigned char)digit;}
	else {throw std::runtime_error ("converting uintxx_t to std::string failed");}
	th -= (digit*divisor);		//std::cout << ret << std::endl;
	}
if (bool(th)) {throw std::runtime_error ("converting uintxx_t to std::string failed");}
return ret;
}

uintxx_t operator+(const uintxx_t& right) const {	//addition
size_t lsize{std::max((*this).size(),right.size())};
uintxx_t ret{};
ret.reserve(lsize);
uint8_t leftover{0};
for ( size_t i{0};i!=lsize;++i) {
	uint8_t val = (*this).at(i)+right.at(i)+leftover;
	ret._d.push_back(val%2);
	leftover=val/2;
	}
if (leftover) {ret._d.push_back(leftover);}
ret.trim();
return ret;
}

uintxx_t operator-(const uintxx_t& right) const {	//subtraction
size_t lsize{std::max((*this).size(),right.size())};
uintxx_t ret{};
ret.reserve(lsize);
uint8_t leftover{0};
for ( size_t i{0};i!=lsize;++i) {
	int8_t val = (*this).at(i)-right.at(i)-leftover;
/*	if (val == 1)		{ret._d.push_back(1);leftover = 0;}
	else if	(val == 0)	{ret._d.push_back(0);leftover = 0;}
	else if	(val == -1)	{ret._d.push_back(1);leftover = 1;}
	else /~(val == -2)~/	{ret._d.push_back(0);leftover = 1;}*/	//this 4 line table is represented in the following 2 lines of code
	leftover = (val<0);
	ret._d.push_back(val%2);
	}
if (leftover) {
	ret._d.push_back(leftover);	//at this point we would have the two's complement value in ret
	throw std::runtime_error ("uintxx_t operator - calculated a negative number ");
	}				//but we will not deal with negitive numbers inside type uintxx_t
ret.trim();
return ret;
}

uintxx_t operator*(const uintxx_t& right) const {	//multiplication
auto left{*this};
uintxx_t ret{};
ret.reserve(this->size()+right.size());
size_t i{right.size()};
do	{
	ret <<= 1;
	if (right.at(--i)) {ret += left;}
	} while (i);
return ret;
}

uintxx_t operator/(const uintxx_t& r) const {	//division
if (this->size()< r.size())	{return 0;}	//if divisor is larger integer division will return zero
if (this->size()==r.size())	{return (*this>=r)?1:0;}
//for (uint8_t i{0};i<=20;++i) 	{if (r == 1<<i) 	{return (*this)>>i;}}	//aditional speedup
uintxx_t rupscale_result{((*this>>1)/r)<<1};		//we will end up here if this->size() > r.size()
if (bool(rupscale_result)) {
	auto leftover = *this - (r * rupscale_result);
	return rupscale_result + (leftover/r);
	}
return (*this>=r)?1:0;	//we will end up here if this->size() > r.size() and (rupscale_result ==0)
}

uintxx_t operator^(uint64_t r) const {	//exponentiation
uintxx_t ret{1};
for (uint64_t n{0};n!=r;++n)	{ret*=(*this);}
return ret;
}

/*
ISO/IEC 14882:2003(E) - 5.6 Multiplicative operators:
The binary / operator yields the quotient, and the binary % operator yields the remainder from the division
of the first expression by the second. If the second operand of / or % is zero the behavior is undefined;
otherwise (a/b)*b + a%b is equal to a. If both operands are nonnegative then the remainder is nonnegative;
if not, the sign of the remainder is implementation-defined 74).
and
74) According to work underway toward the revision of ISO C,
the preferred algorithm for integer division follows the rules defined in the ISO Fortran standard,
ISO/IEC 1539:1991, in which the quotient is always rounded toward zero.
-->	(a/b)*b + a%b == a
-->	a%b == a - (a/b)*b	*/

uintxx_t operator%(const uintxx_t& r) const {return (*this - (*this/r)*r);}	//modulus

uintxx_t operator*(bool r) const {return (r)?uintxx_t{*this}:uintxx_t{};}	//multiplication by single digit binary value {0,1}

bool operator!=(const uintxx_t& r) const {	//operator != might be much faster than operator ==
if (this->size()!=r.size()) {return true;}	//so we define this one first
for (size_t i{0};i!=r.size();++i) 	{if (this->at(i) != r.at(i))	{return true;}}
return false;
}

bool operator<(const uintxx_t& r) const {
if (this->size()<r.size()) 	{return true;}
if (this->size()>r.size()) 	{return false;}
if (r.size()==0)		{return false;}
size_t i{r.size()};
do 	{
	--i;
	if (this->at(i) < r.at(i))	{return true;}
	if (this->at(i) > r.at(i))	{return false;}
	} while (i);
return false;	//if we end up here, *this and r are equal, that implies that *this is not smaller than r
}

uintxx_t operator<<(size_t n) const {		//left shift
auto ret{*this};
ret._d.insert(ret._d.begin(),n,false);
ret.trim();
return ret;
}

uintxx_t operator>>(size_t n) const {		//right shift
if (n >= this->size()) {return uintxx_t{};}	//return zero
auto ret{*this};
ret._d.erase(ret._d.begin(),ret._d.begin()+n);
ret.trim();
return ret;
}

uintxx_t& operator++() {
(*this) += uintxx_t{1};
return *this;
}

uintxx_t& operator--() {
(*this) -= uintxx_t{1};
return *this;
}

uintxx_t operator++(int) {
uintxx_t temp{*this};
++(*this);
return temp;
}

uintxx_t operator--(int) {
uintxx_t temp{*this};
--(*this);
return temp;
}

bool	 operator==(const uintxx_t& r) const 	{return !(*this!=r);}
bool	 operator> (const uintxx_t& r) const 	{return (r<*this);}
bool 	 operator>=(const uintxx_t& r) const	{return ((*this>r) or (*this==r));}
bool 	 operator<=(const uintxx_t& r) const	{return ((*this<r) or (*this==r));}
uintxx_t operator+=(const uintxx_t& r) 		{return (*this)=(*this)+r;}
uintxx_t operator-=(const uintxx_t& r) 		{return (*this)=(*this)-r;}
uintxx_t operator*=(const uintxx_t& r) 		{return (*this)=(*this)*r;}
uintxx_t operator^=(uint64_t r)			{return (*this)=(*this)^r;}
uintxx_t operator/=(const uintxx_t& r) 		{return (*this)=(*this)/r;}
uintxx_t operator%=(const uintxx_t& r) 		{return (*this)=(*this)%r;}
uintxx_t operator<<=(size_t n)			{return (*this)=(*this)<<n;}
uintxx_t operator>>=(size_t n)			{return (*this)=(*this)>>n;}
explicit operator bool() const			{return (*this)!=uintxx_t(0);}	//dont do this; g++ will use it to convert uintxx_t to any integer using bool as intermediary!
};      //class uintxx_t
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
class intxx_t {	//the one int to catch them all
private:	//it can possibly hold any integer value (if your system is fast enough
uintxx_t value;	//should hold its absolute value
int8_t sign;	//should be +1 or -1 for the sign of number

public:
intxx_t(void):value{},sign{1} {};			//default constructor will give us a value of +0
~intxx_t() 				= default;	//default destructor
intxx_t(const intxx_t& in) 		= default;	//default copy
intxx_t& operator=(const intxx_t& in) 	= default;	//default copy assignment
intxx_t(int64_t inp):value{uint64_t(std::abs(inp))},sign{(inp<0)?int8_t{-1}:int8_t{+1}}	{}
intxx_t(const uintxx_t in):value{in},sign(+1) {}

explicit operator int64_t() const{	//convert it back to int64_t
uint64_t ret{value};
auto min{std::numeric_limits<int64_t>::min()};
auto max{std::numeric_limits<int64_t>::max()};
if ((ret>uint64_t(std::abs(min)) or ret>uint64_t(std::abs(max)))) {throw std::runtime_error ("intxx_t operator int64_t() calculated a number that is too large ");}
return (int64_t(ret)*sign);
}

intxx_t operator*(const intxx_t& r) const {	//multiplication
intxx_t ret{};
ret.value = (*this).value* r.value;
ret.sign  = (*this).sign * r.sign;
return ret;
}

intxx_t operator/(const intxx_t& r) const {	//division
intxx_t ret{};
ret.value =((*this).value) / (r.value);
ret.sign  =((*this).sign) * (r.sign);
return ret;
}

intxx_t operator^(size_t r) const {		//exponentiation
intxx_t ret{1};
for (size_t n{0};n!=r;++n)	{ret*=(*this);}
return ret;
}

intxx_t operator-(const intxx_t& r) const {	//subtraction
int8_t right_sign(r.sign*-1);
int8_t left_sign{this->sign};
auto l_abs{this->abs()};
auto r_abs{r.abs()};
const auto& smaller_abs{std::min(l_abs,r_abs)};
const auto& larger_abs {std::max(l_abs,r_abs)};
int8_t larger_sign{(l_abs<r_abs)?right_sign:left_sign};
intxx_t ret{};
ret.value =(left_sign==right_sign)?(l_abs+r_abs):(larger_abs-smaller_abs);
ret.sign = larger_sign;
return ret;
}

intxx_t operator+(const intxx_t& r) const {	//addition
auto neg_r  {r*(intxx_t(-1))};
return ((*this) - neg_r);
}

intxx_t& operator++() {
(*this) += intxx_t{1};
return *this;
}

intxx_t& operator--() {
(*this) -= intxx_t{1};
return *this;
}

intxx_t operator++(int) {
auto temp{*this};
++(*this);
return temp;
}

intxx_t operator--(int) {
auto temp{*this};
--(*this);
return temp;
}

bool operator<(const intxx_t& r) const {
int8_t right_sign(r.sign);
int8_t left_sign{this->sign};
if ((this->value == 0)  && (r.value ==0)) 	{return false;}
if (left_sign/*-1*/ < right_sign/*1*/) 		{return true;}
if (left_sign/*1*/ > right_sign/*-1*/) 		{return false;}
if ((left_sign ==1) && (right_sign ==1))	{return (this->value < r.value);}
if ((left_sign ==-1)&&(right_sign ==-1))	{return (r.value < this->value);}
throw std::runtime_error("intxx_t operator< failed");
}

uintxx_t abs(void) const  			{return value;}	//and ignore sign}
operator std::string() const			{return ((sign == (-1))?"-":"+")+std::string(value);}
intxx_t operator%(const intxx_t& r) const 	{return (*this - (*this/r)*r);}
bool operator> (const intxx_t& r) const		{return r<(*this);}
bool operator!=(const intxx_t& r) const 	{return ((r<(*this)) || ((*this)<r));}
bool operator==(const intxx_t& r) const 	{return !(*this != r);}
intxx_t operator*=(const intxx_t& r) 		{return (*this)=(*this)*r;}
intxx_t operator/=(const intxx_t& r) 		{return (*this)=(*this)/r;}
intxx_t operator^=(size_t r)	 		{return (*this)=(*this)^r;}
intxx_t operator%=(const intxx_t& r) 		{return (*this)=(*this)%r;}
intxx_t operator-=(const intxx_t& r) 		{return (*this)=(*this)-r;}
intxx_t operator+=(const intxx_t& r) 		{return (*this)=(*this)+r;}

};	//class intxx_t
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//////OUTPUT OPERATORS /////////////////////////////////////////////////////////////////////////////
std::ostream& operator << (std::ostream& os, const rom::uintxx_t& v) {
os << std::string(v);
return os;
}

std::ostream& operator << (std::ostream& os, const rom::intxx_t& v) {
os << std::string(v);
return os;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
}       //namespace rom

//otherwise useless test function
template <class ui>	//any integer
ui test_plus_minus(ui in, ui min, ui max) {
ui ret{in};
for (ui i{min}; i!=max; ++i)	{ret += i;}
for (ui i{min}; i!=max; ++i) 	{ret -= i;}
return ret;
}


void rom_int_t(void) {

for (size_t i{0};i!=100;i++) {	//test factorial with our type intxx_t
	rom::intxx_t b{rom::factorial<rom::intxx_t>(i)};
	std::cout << i <<" ! = \t" << b << std::endl;
	}
std::cout << std::endl;



std::cout << "2^4432 - 1 is a mersenne prime number" << std::endl;
rom::intxx_t f{rom::intxx_t{2}^4432};
f--;
std::cout << "it is: "<< std::endl;
std::cout << f << std::endl;

}	//rom_int_t

