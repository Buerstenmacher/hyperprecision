this repository declares and defines two unlimited dynamic integer types
1.      rom::uintxx_t   an unsigned integer with dynamic and possibly unlimited size
2.      rom::intxx_t    an igned integer with dynamic and possibly unlimited size
recomendation:  only use rom::intxx_t type to be safe if negative values come up

--> download all files into one dirctory

--> compile:
g++ example.cpp -o example -std=c++11 -Wno-psabi -Weffc++ -Wall -pedantic -pthread -Os 

--> run:
./example	//you will see a short demo
