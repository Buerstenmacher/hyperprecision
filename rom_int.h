/* this file declares and defines two unlimited dynamic integer types
1.	rom::uintxx_t 	an unsigned integer with dynamic and possibly unlimited size
2. 	rom::intxx_t	an signed integer with dynamic and possibly unlimited size
recomendation:  only use rom::intxx_t type to be safe if negative values come up	*/
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <climits>
#include <limits>

#ifndef ROM_INT
#define ROM_INT

namespace rom {

class uintxx_t;							//there will be a class uintxx_t soon
std::ostream& operator << (std::ostream& os, const ::rom::uintxx_t& v);//there will be an output operator

class intxx_t;							//there will be a class intxx_t soon
std::ostream& operator << (std::ostream& os, const ::rom::intxx_t& v);//there will be an output operator

////////////////////////////////////////////////////////////////////////////////////////////////////
inline uint8_t getbit(uint64_t in, uint8_t nthbit) {//get the value of the nth bit out of one uint64_t
if (nthbit>63) {return 0;}
uint64_t mask{uint64_t(1) << nthbit};
return ((mask & in) >> nthbit);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
class uintxx_t {        //this class should create an unlimitided unsigned integer type
private:				//we will only declare one member variable
mutable std::vector<bool> _d;	//the bit at(n) has a rank of 2^n; there is only this 1 data member

//void reserve(size_t n) {_d.reserve(n+32);}	//32 bits are extra cushion

bool at(size_t nr) const{return (nr<_d.size())?_d[nr]:false;}//access individul bit	//get false==zero if you access bits out of range

size_t trim(void) const	{	//remove all zeroes from _d that are meaningless
static size_t ef{};		//return size in bits
ef = effective_size();
_d.resize(ef,false);
return ef;
}

auto begin(void)	-> decltype(_d.begin()) 	{return _d.begin();}
auto cbegin(void) const -> decltype(_d.cbegin()){return _d.cbegin();}
auto end(void) 		-> decltype(_d.end()) 		{return _d.end();}
auto cend(void)   const -> decltype(_d.cend())	{return _d.cend();}
size_t size(void) const							{return trim();}

size_t decimal_size(void) {	//get the size of the dezimal representation of this number
if ((*this)==0) {return 1;}	//the value of zero is the only one we want to write with a leading zero
size_t digits{0};
do 	{++digits;} while(ten_pow(digits) <= *this);
return digits;
}

explicit operator unsigned char() const{	//convert it back to uint8_t
uint64_t uchar_n_values{UCHAR_MAX+1};
return uint64_t((*this)%uchar_n_values);
}

public:
uintxx_t(void):_d{} {};			//default constructor; empty _d means value is zero
~uintxx_t() = default;			//default destructor
uintxx_t(const uintxx_t& in) = default;	//default copy
uintxx_t& operator=(const uintxx_t& in) = default;//default copy assignment

static uintxx_t ten_pow(const size_t n) {	//should calculate 10^n as fast as possible
static std::vector<uintxx_t> mem{1,10,100,1000};	//example: ten_pow(3) will return 1000
while (n>=mem.size())	{mem.push_back(mem.back()*10);}
return mem.at(n);
}

size_t effective_size(void) const	{	//get the size in bits of this number
size_t s{_d.size()};
while(s) {if(this->_d[--s]) {return(s+1);}} //ignore leading zeros
return (0);
}

uintxx_t(uint64_t inp):_d{} {		//construct from built in uint64_t
for (uint8_t i{0};i!=64;++i) {_d.push_back(getbit(inp,i));}
trim();
}

explicit operator uint64_t() const{	//convert it back to uint64_t
if (effective_size()>64)	{throw std::runtime_error ("a number of type uintxx_t is to large to be converted to uint64_t");}
uint64_t res{0};
size_t i{effective_size()};
while (i) {
	res <<= 1;
	res += _d.at(--i);
	}
return res;
}

operator std::string() const{		//convert to decimal std::string
if ((*this)==0) {return "0";}
std::string ret{};	//return value
auto th {*this};	//copy
for (auto sz{th.decimal_size()};sz!=0;sz--) {
	auto divisor{ten_pow(sz-1)};
	auto digit{th/divisor};
	if ((digit <= 9)&&(digit>=0))	{ret += '0'+ (unsigned char)digit;}
	else {throw std::runtime_error ("converting uintxx_t to std::string failed");}
	th -= (digit*divisor);		//std::cout << ret << std::endl;
	}
if (bool(th)) {throw std::runtime_error ("converting uintxx_t to std::string failed");}
return ret;
}

uintxx_t operator+=(const uintxx_t& r) 		{//zero copy inplace
if(this->_d.size()<r._d.size()) {this->_d.resize(r._d.size(),false);}
static auto r_it{r.cbegin()};		r_it = r.cbegin();
static auto l_it{this->begin()};	l_it = this->begin();
static auto r_end{r.cend()};		r_end = r.cend();
static auto l_end{this->end()};		l_end = this->end();
static uint8_t leftover{};
static uint8_t val{};
while (r_it!=r_end) {
	val = (*l_it) + (*r_it++) + leftover;
	leftover = (val/2);
	(*l_it++)=(val%2);
	}
while (l_it!=l_end) {
	val = (*l_it) + leftover;
	leftover = (val/2);
	(*l_it++)=(val%2);
	}
while (leftover) {
	val = leftover;
	leftover = (val/2);
	this->_d.push_back(val%2);
	}
return *this;
}

uintxx_t operator-=(const uintxx_t& r)	{//zero copy
auto& th{*this};
if (th<r) {throw std::runtime_error ("uintxx_t operator - calculated a negative number (top)");}
auto r_it{r.cbegin()};
auto l_it{th.begin()};
auto r_end{r.cend()};
auto l_end{th.end()};
uint8_t leftover{0};
while (r_it!=r_end) {
	int8_t val = (*l_it) - (*r_it++) -leftover;
	leftover = (val<0);
	(*l_it++)=(val%2);
	}
while (l_it!=l_end) {
	int8_t val = (*l_it) - leftover;
	leftover = (val<0);
	(*l_it++)=(val%2);
	}
while (leftover) {
	uint8_t val = leftover;
	leftover = (val/2);
	th._d.push_back((val%2)?1:0);//at this point we would have the two's complement value in ret
	throw std::runtime_error ("uintxx_t operator - calculated a negative number (bottom)");
	}			//but we will not deal with negitive numbers inside type uintxx_t
th.trim();
return th;
}

uintxx_t operator+(const uintxx_t& right) const {	//addition
auto th{*this};
return th+=right;
}

uintxx_t operator-(const uintxx_t& right) const {	//subtraction
auto th{*this};
return th-=right;
}

uintxx_t operator*(const uintxx_t& right) const {	//multiplication
uintxx_t ret{};
size_t i{right.effective_size()};
while (i)	{
	ret <<= 1;
	if (right._d[--i]) {ret += (*this);}
	}
return ret;
}

uintxx_t operator/(const uintxx_t& r) const {	//recursive division algorithm
if (this->effective_size()< r.effective_size())	{return 0;}	//if divisor is larger integer division will return zero
if (this->effective_size()==r.effective_size())	{return (*this>=r)?1:0;}
if (r == 2) 					{return (*this)>>1;}	//aditional speedup
uintxx_t rupscale_result{((*this>>1)/r)<<1};		//we will end up here if this->effective_size() > r.effective_size()
if (bool(rupscale_result)) {
	auto leftover = *this - (r * rupscale_result);
	return rupscale_result + (leftover/r);
	}
return (*this>=r)?1:0;	//we will end up here if this->effective_size() > r.effective_size() and (rupscale_result ==0)
}

uintxx_t operator^(uint64_t r) const {	//exponentiation
uintxx_t ret{1};
for (uint64_t n{0};n!=r;++n)	{ret*=(*this);}
return ret;
}

/*	ISO/IEC 14882:2003(E) - 5.6 Multiplicative operators:
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

/*bool operator!=(const uintxx_t& r) const {	//operator != might be much faster than operator ==
if (this->effective_size()!=r.effective_size()) {return true;}	//so we define this one first
for (size_t i{0};i!=r.effective_size();++i) 	{if (this->at(i) != r.at(i))	{return true;}}
return false;
}*/

bool operator<(const uintxx_t& r) const {
auto i(r.effective_size());
const auto h(effective_size());
if (h < i) 	{return true;}
if (h > i) 	{return false;}
while (i--) {
	if (this->at(i) < r.at(i))	{return true;}
	if (this->at(i) > r.at(i))	{return false;}
	}
return false;	//if we end up here, *this and r are equal, that implies that *this is not smaller than r
}

uintxx_t operator<<=(size_t n)			{
this->_d.insert(this->_d.begin(),n,false);
this->trim();
return *this;
}

uintxx_t operator>>=(size_t n)			{
this->_d.erase(this->_d.begin(),this->_d.begin()+n);
this->trim();
return *this;
}

uintxx_t operator<<(size_t r) const {
auto th{*this};
return th<<=r;
}

uintxx_t operator>>(size_t r) const {
auto th{*this};
return th>>=r;
}

uintxx_t& operator++() {
static auto one{uintxx_t{1}};
(*this) += one;
return *this;
}

uintxx_t& operator--() {
static auto one{uintxx_t{1}};
(*this) -= one;
return *this;
}

uintxx_t operator++(int) {
auto temp{*this};
++(*this);
return temp;
}

uintxx_t operator--(int) {
auto temp{*this};
--(*this);
return temp;
}

bool 	 operator!=(const uintxx_t& r) const 	{return ((*this<r) or (*this>r));}
bool	 operator==(const uintxx_t& r) const 	{return !(*this!=r);}
bool	 operator> (const uintxx_t& r) const 	{return (r<*this);}
bool 	 operator>=(const uintxx_t& r) const	{return (!(*this<r));}//todo: this could be faster
bool 	 operator<=(const uintxx_t& r) const	{return (!(*this>r));}//todo: this could be faster
uintxx_t operator*=(const uintxx_t& r) 		{return (*this)=(*this)*r;}
uintxx_t operator^=(uint64_t r)			{return (*this)=(*this)^r;}
uintxx_t operator/=(const uintxx_t& r) 		{return (*this)=(*this)/r;}
uintxx_t operator% (const uintxx_t& r) const 	{return (*this - (*this/r)*r);}	//modulus
uintxx_t operator%=(const uintxx_t& r) 		{return (*this)=(*this)%r;}
explicit operator bool() const			{return (*this)!=uintxx_t(0);}	//dont do this; g++ will use it to convert uintxx_t to any integer using bool as intermediary!
};      //class uintxx_t
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
class intxx_t {	//the one int to catch them all
private:	//it can possibly hold any integer value (if your system is fast enough)
uintxx_t value;	//should hold its absolute value
int8_t sign;	//should be +1 or -1 for the sign of number

public:
intxx_t(void):value{},sign{1} {};			//default constructor will give us a value of +0
~intxx_t() 				= default;	//default destructor
intxx_t(const intxx_t& in) 		= default;	//default copy
intxx_t& operator=(const intxx_t& in) 	= default;	//default copy assignment

intxx_t(int64_t inp):value{uint64_t(std::abs(inp))},sign{(inp<0)?int8_t(-1):int8_t(+1)} {}

intxx_t(const uintxx_t in):value{in},sign(+1) {}

explicit operator int64_t() const{	//convert it back to int64_t
uint64_t ret = uint64_t{value};
auto min{std::numeric_limits<int64_t>::min()};
auto max{std::numeric_limits<int64_t>::max()};
if ((ret>uint64_t(std::abs(min)) or ret>uint64_t(std::abs(max)))) {throw std::runtime_error ("intxx_t operator int64_t() calculated a number that is too large ");}
return (int64_t(ret)*sign);
}

size_t effective_size(void) const	{return value.effective_size();}

intxx_t operator*(const intxx_t& r) const {	//multiplication
intxx_t ret{};
ret.value = (*this).value * r.value;
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
const auto& l_abs{this->abs()};
const auto& r_abs{r.abs()};
const auto& smaller_abs{std::min(l_abs,r_abs)};
const auto& larger_abs {std::max(l_abs,r_abs)};
int8_t larger_sign{(l_abs<r_abs)?right_sign:left_sign};
intxx_t ret{};
ret.value =(left_sign==right_sign)?(l_abs+r_abs):(larger_abs-smaller_abs);
ret.sign = larger_sign;
return ret;
}

intxx_t operator+(const intxx_t& r) const {	//addition
auto neg_r{r};
neg_r.sign *= -1;
return ((*this) - neg_r);
}

intxx_t& operator++() {
static intxx_t one{1};
(*this) += one;
return *this;
}

intxx_t& operator--() {
static intxx_t one{1};
(*this) -= one;
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

intxx_t operator<<(size_t n) const 		{return intxx_t{value<<n}*intxx_t{sign};}
intxx_t operator>>(size_t n) const 		{return intxx_t{value>>n}*intxx_t{sign};}
intxx_t operator<<=(size_t n)			{return (*this)=(*this)<<n;}
intxx_t operator>>=(size_t n)			{return (*this)=(*this)>>n;}

uintxx_t abs(void) const  			{return value;}	//and ignore sign}
operator std::string() const			{return ((sign == (-1))?"-":"+")+std::string(value);}
intxx_t operator% (const intxx_t& r) const 	{return (*this - (*this/r)*r);}
bool 	operator> (const intxx_t& r) const	{return r<(*this);}
bool 	operator!=(const intxx_t& r) const 	{return ((r<(*this)) || ((*this)<r));}
bool 	operator==(const intxx_t& r) const 	{return !(*this != r);}
intxx_t operator*=(const intxx_t& r) 		{return (*this)=(*this)*r;}
intxx_t operator/=(const intxx_t& r) 		{return (*this)=(*this)/r;}
intxx_t operator^=(size_t r)	 		{return (*this)=(*this)^r;}
intxx_t operator%=(const intxx_t& r) 		{return (*this)=(*this)%r;}
intxx_t operator-=(const intxx_t& r) 		{return (*this)=(*this)-r;}
intxx_t operator+=(const intxx_t& r) 		{return (*this)=(*this)+r;}
explicit operator bool() const			{return (*this)!=intxx_t(0);}	//dont do this; g++ will use it to convert uintxx_t to any integer using bool as intermediary!
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

#endif
