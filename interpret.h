/*
 *  Project name:
 *  Implementace interpretu imperativního jazyka IFJ15
 *
 *  Date: 13.12.2015
 *
 *  Repository:
 *  https://github.com/kuroimarimo/imperative-language-interpreter
 *
 *  Team:
 *  Vojtěch Václavík	(xvacla22)
 *  Petr Vančo			(xvanco05)
 *  Filip Vaško         (xvasko10)
 *  Dominik Vašek		(xvasek06)
 *  Valentína Straková	(xstrak27)
 */

#ifndef INTERPRET_H
#define INTERPRET_H

#include "instructions.h"
#include "error.h"
#include "frames.h"
#include "3ak.h"
#include "ial.h"
#include "lex_an.h"

void interpret(tInstruction * instruction);
static inline char * inputString();

#endif
