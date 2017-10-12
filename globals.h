#ifndef GLOBALS_H
#define GLOBALS_H


#include "Beallitasok.h"
#include "zlog.h"

#define zforeach(var, container) for(auto var = (container).begin(); var != (container).end(); ++var)

extern Beallitasok beallitasok;
extern zLog zlog;

#endif // GLOBALS_H
