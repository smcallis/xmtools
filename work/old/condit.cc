#include <stdio.h>
#include <string.h>
#include <typeinfo>

template<class atype, class btype, class ctype>
void t2(const char* aname, const char* bname, const char* cname) {
    atype aval = 0;
    btype bval = 0;
    ctype cval = 0;
    int got = strcmp(typeid(true ? aval : bval).name(), typeid(cval).name());
    if (got == 0) {
        printf("    template<> struct conditional<%s, %s> { typedef %s type; };\n", aname, bname, cname);

    }
}

template<class atype, class btype>
void t1(const char* aname, const char* bname) {
    t2<atype, btype,               char>(aname, bname, "              char");
    t2<atype, btype,        signed char>(aname, bname, "       signed char");
    t2<atype, btype,      unsigned char>(aname, bname, "     unsigned char");
    t2<atype, btype,       signed short>(aname, bname, "      signed short");
    t2<atype, btype,     unsigned short>(aname, bname, "    unsigned short");
    t2<atype, btype,         signed int>(aname, bname, "        signed int");
    t2<atype, btype,       unsigned int>(aname, bname, "      unsigned int");
    t2<atype, btype,        signed long>(aname, bname, "       signed long");
    t2<atype, btype,      unsigned long>(aname, bname, "     unsigned long");
    t2<atype, btype,   signed long long>(aname, bname, "  signed long long");
    t2<atype, btype, unsigned long long>(aname, bname, "unsigned long long");
    t2<atype, btype,              float>(aname, bname, "             float");
    t2<atype, btype,             double>(aname, bname, "            double");
    t2<atype, btype,        long double>(aname, bname, "       long double");
}

template<class atype>
void t0(const char* aname) {
    t1<atype,               char>(aname, "              char");
    t1<atype,        signed char>(aname, "       signed char");
    t1<atype,      unsigned char>(aname, "     unsigned char");
    t1<atype,       signed short>(aname, "      signed short");
    t1<atype,     unsigned short>(aname, "    unsigned short");
    t1<atype,         signed int>(aname, "        signed int");
    t1<atype,       unsigned int>(aname, "      unsigned int");
    t1<atype,        signed long>(aname, "       signed long");
    t1<atype,      unsigned long>(aname, "     unsigned long");
    t1<atype,   signed long long>(aname, "  signed long long");
    t1<atype, unsigned long long>(aname, "unsigned long long");
    t1<atype,              float>(aname, "             float");
    t1<atype,             double>(aname, "            double");
    t1<atype,        long double>(aname, "       long double");
}


int main() {
    t0<              char>("              char"); printf("\n");
    t0<       signed char>("       signed char"); printf("\n");
    t0<     unsigned char>("     unsigned char"); printf("\n");
    t0<      signed short>("      signed short"); printf("\n");
    t0<    unsigned short>("    unsigned short"); printf("\n");
    t0<        signed int>("        signed int"); printf("\n");
    t0<      unsigned int>("      unsigned int"); printf("\n");
    t0<       signed long>("       signed long"); printf("\n");
    t0<     unsigned long>("     unsigned long"); printf("\n");
    t0<  signed long long>("  signed long long"); printf("\n");
    t0<unsigned long long>("unsigned long long"); printf("\n");
    t0<             float>("             float"); printf("\n");
    t0<            double>("            double"); printf("\n");
    t0<       long double>("       long double"); printf("\n");
}
