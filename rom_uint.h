/* this file declares and defines two unlimited dynamic integer types
1.      rom::uintxx_t   an unsigned integer with dynamic and possibly unlimited size
2.      rom::intxx_t    an signed integer with dynamic and possibly unlimited size
recomendation:  only use rom::intxx_t type to be safe if negative values come up        */
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <climits>
#include <limits>
#include <type_traits>

#ifndef ROM__INT
#define ROM__INT

namespace rom {

//class uintxx__t;	//there will be a class uintxx_t soon
//std::ostream& operator << (std::ostream& os, const ::rom::uintxx__t& v);//there will be an output operator

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
template <class uint = uint8_t>
class uintxx__t {        	//this class should create an unlimitided unsigned integer type
private:                        //we will only declare one member variable
mutable std::vector<uint> _d;   //the uint at(n) has a rank of 2^(n*uint_bits())

static uint uint_max(void)	{return std::numeric_limits<uint>::max();}//calculate maximum value for uint
static size_t uint_bits(void)	{return sizeof(uint)*CHAR_BIT;}		  //calculate size in bits of uint

void trim(void) const {while (_d.size() && !_d.back()) {_d.pop_back();}}

uint at(size_t nr) const{return (nr<_d.size())?_d[nr]:uint{0};}//access individul element //get 0 if you access bits out of range

static uint8_t predict_overflow(uint a, uint b) 	{return  ((uint_max()-a) < b)?1:0; }
static uint8_t predict_underflow(uint a, uint b)	{return  (a < b)?1:0; }

void shift_r_elem(size_t n) {	//right shift entire elements
_d.erase(_d.begin(),_d.begin()+n);
}

void shift_l_elem(size_t n) {	//left shift entire elements
_d.insert(_d.begin(),n,uint{0});
}

static uint create_mask_lbit(size_t n) {	// example 0b 0001 1111 if uint = uint8_t and n = 5
uint val{1};
val <<= n;
return (--val);
}

static uint create_mask_hbit(size_t n) {return ~create_mask_lbit(n);}

void shift_r_bit(size_t n) {	//right shift n bits
if (n>=uint_bits()) {throw std::runtime_error(">> error");}
if (!n) {return;}
auto it{_d.end()};
uint lo{0},hi{0},oldlo{0};
while (it-- != _d.begin()){
	oldlo = lo;
	lo = (create_mask_lbit(n) & *it);
	hi = (create_mask_hbit(n) & *it);
	*it = hi >> n;
	*it |= oldlo << (uint_bits()-n);
	}
trim();
}

void shift_l_bit(size_t n) {	//left shift n bits
if (n>=uint_bits()) {throw std::runtime_error("<< error");}
if (!n) {return;}
auto it{_d.begin()};
uint lo{0},hi{0},oldhi{0};
while(it != _d.end()) {
	oldhi = hi;
	lo = (*it & create_mask_lbit(uint_bits()-n));
	hi = (*it & create_mask_hbit(uint_bits()-n));
	*it = lo  << n;
	*it |= oldhi >> (uint_bits()-n);
	it++;
	}
_d.push_back(hi >> (uint_bits()-n));
trim();
}

size_t effective_size(void) const       {//get the size in bits of this number
trim();
if (_d.size()==0) {return 0;}
const auto last{_d.back()};
size_t e{0};
for ( ;e!=uint_bits();++e) {if ((last >> e)==0) {break;}}
return e+(_d.size()-1)*uint_bits();
}

ptrdiff_t largest_signif_dig(void) const {	//we would like to know where the first digit >0 is in this number
trim();
return ptrdiff_t(_d.size()) - 1;
}

public:
uintxx__t(void):_d{} {};                 //default constructor; empty _d means value is zero
~uintxx__t() = default;                  //default destructor
uintxx__t(const uintxx__t& in) = default; //default copy
uintxx__t& operator=(const uintxx__t& in) = default;//default copy assignment
uintxx__t(uint in):_d{}	{_d.push_back(in);}

static uintxx__t multiply(uint l,uint r) {
size_t halfsz{uint_bits()/2};
uint lupp{uint((l & create_mask_hbit(halfsz))>>halfsz)};
uint llow{uint(l & create_mask_lbit(halfsz))};
uint rupp{uint((r & create_mask_hbit(halfsz))>>halfsz)};
uint rlow{uint(r & create_mask_lbit(halfsz))};
uintxx__t a{uint(lupp*rupp)};
uintxx__t b{uint(lupp*rlow)};
uintxx__t c{uint(llow*rupp)};
uintxx__t d{uint(llow*rlow)};
uintxx__t ret{(a<<uint_bits()) + (b<<halfsz) + (c<<halfsz) + d};
return ret;
}

uintxx__t operator*=(uint r) {
uintxx__t ret{0};
for (size_t i{0};i!=_d.size();++i) {
	ret += (multiply(_d.at(i),r) << (i*uint_bits()));
	}
*this = ret;
return *this;
}

uintxx__t operator*=(const uintxx__t& r) {
uintxx__t ret{0};
for (size_t i{0};i!=_d.size();++i) {
	ret += (r*_d.at(i)) << (i*uint_bits());
	}
*this = ret;
return *this;
}

uintxx__t div_one_digit(const uintxx__t& b)	{
if (this->effective_size()>uint_bits()) {throw std::runtime_error ("divisor is larger than one digit!");}
if (b.effective_size()>uint_bits()) 	{throw std::runtime_error ("dividend is larger than one digit!");}
return (this->_d.front()/b._d.front());
}

uintxx__t operator/(const uintxx__t& r) const {
}

uintxx__t operator<<=(size_t n)	{
shift_l_bit(n%uint_bits());
shift_l_elem(n/uint_bits());
return *this;
}

uintxx__t operator>>=(size_t n)	{
shift_r_elem(n/uint_bits());
shift_r_bit(n%uint_bits());
return *this;
}

bool operator<(const uintxx__t& r) const {
auto rs(r.effective_size());
const auto ls(effective_size());
if (ls < rs)      {return true;}
if (ls > rs)      {return false;}
for (size_t i{_d.size()} ; i ; --i) {
        if (this->at(i) < r.at(i))      {return true;}
        if (this->at(i) > r.at(i))      {return false;}
        }
return false;   //if we end up here, *this and r are equal, that implies that *this is not smaller than r
}

bool     operator!=(const uintxx__t& r) const    {return ((*this<r) or (*this>r));}
bool     operator==(const uintxx__t& r) const    {return !(*this!=r);}
bool     operator> (const uintxx__t& r) const    {return (r<*this);}
bool     operator>=(const uintxx__t& r) const    {return (!(*this<r));}//todo: this could be faster
bool     operator<=(const uintxx__t& r) const    {return (!(*this>r));}//todo: this could be faster
explicit operator bool() const                  {return (*this)!=uintxx__t(0);}  //dont do this; g++ will use it to convert uintxx_t to any integer using bool as intermediary!


uintxx__t operator+=(const uintxx__t& r)          {//zero copy inplace
if(this->_d.size()<r._d.size()) {this->_d.resize(r._d.size(),uint(0));}
auto r_it{r._d.cbegin()};
auto l_it{this->_d.begin()};
auto r_end{r._d.cend()};
auto l_end{this->_d.end()};
uint leftover{0};
while (r_it!=r_end) {
	uint8_t overflow = predict_overflow(*l_it,*r_it);	//predict integer overflow
	uint val = *l_it + *r_it; 				//add them anyway
	overflow += predict_overflow(val,leftover);	//predict integer overflow
	val+= leftover;					//add them anyway
	leftover = overflow;				//store overflows for next iterations
	*l_it++ = val;
	r_it++;
       }
while (l_it!=l_end) {
	uint8_t overflow = predict_overflow(*l_it,leftover);	//predict integer overflow
	uint val = *l_it + leftover;				//add them anyway
	leftover = overflow;				//store overflows for next iterations
	*l_it++ = val;
       }
if (leftover) {	this->_d.push_back(leftover);}
trim();
return *this;
}

uintxx__t operator-=(const uintxx__t& r)          {//zero copy inplace
auto& th{*this};
if (th<r) {throw std::runtime_error ("uintxx__t operator - calculated a negative number (top)");}
auto r_it{r._d.cbegin()};
auto l_it{th._d.begin()};
auto r_end{r._d.cend()};
auto l_end{th._d.end()};
int8_t leftover{0};
while (r_it!=r_end) {
	int8_t underflow = predict_underflow(*l_it,*r_it);
	uint val = *l_it - *r_it;
	underflow += predict_underflow(val,leftover);
	val -= leftover;
	leftover = underflow;
	*l_it++ = val;
	r_it++;
	}
while (l_it!=l_end)  {
	int8_t underflow = predict_underflow(*l_it,leftover);
	uint val = *l_it - leftover;
	leftover = underflow;
	*l_it++ = val;
	}
if (leftover) {	throw std::runtime_error("uintxx__t operator - calculated a negative ");}
trim();
return *this;
}

uintxx__t operator+(const uintxx__t& right) const {       //addition
auto th{*this};
return th+=right;
}

uintxx__t operator-(const uintxx__t& right) const {       //subtraction
auto th{*this};
return th-=right;
}

uintxx__t operator*(uint r) const {			//multiplication by uint
auto th{*this};
return th*=r;
}

uintxx__t operator*(const uintxx__t r) const {			//multiplication by uintxx__t
auto th{*this};
return th*=r;
}

uintxx__t operator<<(const size_t& right) const {       //left shift
auto th{*this};
return th<<=right;
}

uintxx__t operator>>(const size_t& right) const {       //right shift
auto th{*this};
return th>>=right;
}

uintxx__t& operator++() {
static auto one{uintxx__t{1}};
(*this) += one;
return *this;
}

uintxx__t& operator--() {
static auto one{uintxx__t{1}};
(*this) -= one;
return *this;
}

uintxx__t operator++(int) {
auto temp{*this};
++(*this);
return temp;
}

uintxx__t operator--(int) {
auto temp{*this};
--(*this);
return temp;
}

operator std::string() const{
if (_d.size() == 0)	{return std::string("0");}
std::string ret{};
for (auto& val:_d) {ret = std::to_string(val)+std::string(" ") + ret;}
//for (auto it{_d.rbegin()}; it!=_d.rend(); ++it) {ret = std::to_string(*it)+std::string(" ") + ret;}
return ret;
}

}; //class uintxx__t

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//////OUTPUT OPERATORS /////////////////////////////////////////////////////////////////////////////
template <class ui>
std::ostream& operator << (std::ostream& os, const rom::uintxx__t<ui>& v) {
os << std::string(v);
return os;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
}       //namespace rom

#endif
