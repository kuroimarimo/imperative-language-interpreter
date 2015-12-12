#ifndef INTERPRET_H
#define INTERPRET_H

#include "instructions.h"
#include "error.h"
#include "frames.h"
#include "3ak.h"
#include "ial.h"
#include "lex_an.h"

char * inputString();
void interpret(tInstruction * instruction);

#endif