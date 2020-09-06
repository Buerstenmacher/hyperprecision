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
using temp_type = floatxx_t<precision + 16>;	//some temporary values will be stored with extra precision
intxx_t _exp;    //exponent to the base 2.0
intxx_t _mant;   //mantissa;

template <size_t p>
friend auto log(const floatxx_t<p>& xin,bool) -> floatxx_t<p>;

template <size_t p>
friend auto exp(const floatxx_t<p>& xin) -> floatxx_t<p>;

template <size_t p>
friend floatxx_t<p> log(const floatxx_t<p>& x,const floatxx_t<p>& bas);//overload for arbitrary base

void shift_mantissa_right(int64_t digits) {     //if digits is negative shift left //round or cut of if right shift
if      (digits>0) {
	/*if (!round)*/	{_mant >>= digits;}
//	if (round) 	{_mant.shift_right_and_round(digits);}//right shift with positive input should preserve value but precision might be lost	
	}
else if (digits<0)      {_mant <<= std::abs(digits);}		//left shift with negative input will always preserve value and precision	
_exp += digits;
}

/*
///////////////////round
Googol is 10^100 =      +10.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000e+99
Googolplex is 10^Googol =       +9.99999999999999999999999999999999999999999999999999999947192967845420839310419193860940028249906558987253579396899249742079429879e+9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999
lns      (5 / 50)+0.9 = +1.00000000000000020000000000000001e+0 
e^(0.001) is:   +1.0010005001667083e+0
e^(0.01) is:    +1.0100501670841681e+0
e^(0.1) is:     +1.1051709180756476e+0
e^(1) is:       +2.7182818284590453e+0
e^(10) is:      +2.2026465794806722e+4
e^(100) is:     +2.6881171418161417e+43
e^(1000) is:    +1.9700711140170932e+434
e^(10000) is:   +8.8068182256649860e+4342
e^(100000) is:  +2.8066633604326965e+43429
e^(1000000) is: +3.0332153968731115e+434294
e^(10000000) is:        +6.5922325361599772e+4342944
The lns<> number that is nearest to 5 is:       +5.00000000000000072463689669365529e+0
The lns<> number that is nearest to -5 is:      -5.00000000000000022463689669365527e+0
The lns<> number that is nearest to 0 is:       +2.71670189268954687356097802335295e-401
The smallest lns<> number is:   -3.68093386576911271980432736383401e+400
The largest  lns<> number is:   +3.68093386576911235171094078692278e+400
(2^3217- 1) is a mersenne prime number
it is: 
+259117086013202627776246767922441530941818887553125427303974923161874019266586362086201209516800483406550695241733194177441689509238807017410377709597512042313066624082916353517952311186154862265604547691127595848775610568757931191017711408826252153849035830401185072116424747461823031471398340229288074545677907941037288235820705892351068433882986888616658650280927692080339605869308790500409503709875902119018371991620994002568935113136548829739112656797303241986517250116412703509705427773477972349821676443446668383119322540099648994051790241624056519054483690809616061625743042361721863339415852426431208737266591962061753535748892894599629195183082621860853400937932839420261866586142503251450773096274235376822938649407127700846077124211823080804139298087057504713825264571448379371125032081826126566649084251699453951887789613650248405739378594599444335231188280123660406262468609212150349937584782292237144339628858485938215738821232393687046160677362909315071
it is: 
+2.59117086013205305068494724178244e+968
e    is:        +2.71828182845904523536028747135266e+0
pi   is:        +3.14159265358979323846264338327950e+0
ln(e^(1.0)) is: +10.0000000000000000e-1
ln(e^(10.0)) is:        +1.0000000000000000e+1
ln(e^(100.0)) is:       +1.0000000000000000e+2
ln(e^(1000.0)) is:      +1.0000000000000000e+3
ln(e^(10000.0)) is:     +1.0000000000000000e+4
ln(e^(100000.0)) is:    +1.0000000000000000e+5
ln(e^(1000000.0)) is:   +1.0000000000000000e+6
ln(ln(ln(e^(e^(e^(4.0)))))) is: +4.0000000000000001e+0   Try this calculation on any other calculator!!!
arcsin(sin(0.8))is:     +8.00000000000000000000000000000000e-1
arccos(cos(0.8))is:     +8.00000000000000000000000000000000e-1
cos(20.0)      is:      +4.08082061813391986062267860927645e-1
cos(20)*cos(20) + sin(20)*sin(20)  is:
+1.07e+0          8bit Float 
+1.0007e+0       16bit Float 
+1.00000000e-0   32bit Float 
+9.9999999999999996e-1   64bit Float




////////////////////////////////////////////Cutoff
Googol is 10^100 =      +1.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000e+100
Googolplex is 10^Googol =       +.99999999999999999999999999999999999999999999999999999955930300154336119084693123403557936770074931485380580243957088941005289350e+10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
lns      (5 / 50)+0.9 = +10.00000000000000000000000000000000e-1 
e^(0.001) is:   +1.0010005001667083e+0
e^(0.01) is:    +1.0100501670841680e+0
e^(0.1) is:     +1.1051709180756476e+0
e^(1) is:       +2.7182818284590454e+0
e^(10) is:      +2.2026465794806746e+4
e^(100) is:     +2.6881171418161726e+43
e^(1000) is:    +1.9700711140173198e+434
e^(10000) is:   +8.8068182256751236e+4342
e^(100000) is:  +2.8066633604650136e+43429
e^(1000000) is:         +3.0332153972223156e+434294
e^(10000000) is:        +6.5922325437525035e+4342944
The lns<> number that is nearest to 5 is:       +4.99999999999999972463615927498315e+0
The lns<> number that is nearest to -5 is:      -4.99999999999999922463615927498323e+0
The lns<> number that is nearest to 0 is:       +2.71670189268954687377722796764474e-401
The smallest lns<> number is:   -3.68093386576911271949228067514084e+400
The largest  lns<> number is:   +3.68093386576911235139889409822961e+400
(2^3217- 1) is a mersenne prime number
it is: 
+259117086013202627776246767922441530941818887553125427303974923161874019266586362086201209516800483406550695241733194177441689509238807017410377709597512042313066624082916353517952311186154862265604547691127595848775610568757931191017711408826252153849035830401185072116424747461823031471398340229288074545677907941037288235820705892351068433882986888616658650280927692080339605869308790500409503709875902119018371991620994002568935113136548829739112656797303241986517250116412703509705427773477972349821676443446668383119322540099648994051790241624056519054483690809616061625743042361721863339415852426431208737266591962061753535748892894599629195183082621860853400937932839420261866586142503251450773096274235376822938649407127700846077124211823080804139298087057504713825264571448379371125032081826126566649084251699453951887789613650248405739378594599444335231188280123660406262468609212150349937584782292237144339628858485938215738821232393687046160677362909315071
it is: 
+2.59117086013200122726745872586341e+968
e    is:        +2.71828182845904523536028747135266e+0
pi   is:        +3.14159265358979323846264338327950e+0
ln(e^(1.0)) is: +10.0000000000000000e-1
ln(e^(10.0)) is:        +1.0000000000000001e+1
ln(e^(100.0)) is:       +1.0000000000000001e+2
ln(e^(1000.0)) is:      +1.0000000000000001e+3
ln(e^(10000.0)) is:     +1.0000000000000001e+4
ln(e^(100000.0)) is:    +1.0000000000000001e+5
ln(e^(1000000.0)) is:   +1.0000000000000001e+6
ln(ln(ln(e^(e^(e^(4.0)))))) is: +4.0000000000000005e+0   Try this calculation on any other calculator!!!
arcsin(sin(0.8))is:     +7.99999999999999999999999999999999e-1
arccos(cos(0.8))is:     +7.99999999999999999999999999999999e-1
cos(20.0)      is:      +4.08082061813391986062267860927644e-1
cos(20)*cos(20) + sin(20)*sin(20)  is:
+.98e-0   8bit Float 
+9.9658e-1       16bit Float 
+9.99999896e-1   32bit Float 
+9.9999999999999977e-1   64bit Float 

*/

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

template <size_t dig>     //intended numer of bits representing the mantissa
friend class floatxx_t;
template <size_t dig>     //intended numer of bits representing the mantissa
explicit floatxx_t(const floatxx_t<dig>& in):_exp{in._exp},_mant{in._mant} {renormalize();}//conversion from this type with different template parameters

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
//tmp.renormalize();
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

