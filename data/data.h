#include <inc.h>
#include <string>
#ifndef DATA_H
#define DATA_H

const struct company com[]{
    {"google",
     "www.google.com",
     "123456789",
     "google@gmail.com",
     "google.com"},
    {"facebook",
     "www.facebook.com",
     "987654321",
     "facebool@gmail.com",
     "facebook.com"},
    {"microsoft",
     "www.microsoft.com",
     "123456789",
     "microsoft@outlook.com",
     "microsoft.com"},
    {"apple",
     "www.apple.com",
     "987654321",
     "apple@gmail.com",
     "apple.com"}};

const std::string names[] = {"pepe", "juan", "luis", "paco", "jose", "lucas", "maria", "lucia", "luisa", "paca"};
const std::string surnames[]= {"garcia", "fernandez", "lopez", "martinez", "sanchez", "perez", "gomez", "martin", "jimenez", "ruiz"};
const std::string ages[]= {"20", "21", "22", "23", "24", "25", "26", "27", "28", "29"};
const std::string dnis[]= {"73214521", "12214521", "73214521", "73298721", "74564521", "73211231"};
const std::string adresses[]= {"calle1", "calle2", "calle3", "calle4", "calle5", "calle6", "calle7", "calle8", "calle9", "calle10"};
const std::string phones[]= {"97214521", "922167821", "93214521", "93298788", "94564599", "93211255"};
const std::string emails[]= {"chichico@gmail.com","lolo12@outlook.com","example1@gmail.com","example2@gmail.com","asfbasoffice@gmail.com"};
const std::string jobs[]= {"developer", "tester", "manager", "director", "ceo", "cto", "cfo", "coo", "cso", "cdo"};
const std::string companys[]= {"google", "facebook", "microsoft", "apple"};

#endif