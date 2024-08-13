//
// Created by christophe on 6/30/24.
//

#include "xAssert.h"

#include <stdio.h>
#include <stdlib.h>

/*
 * xAssert
 */
void xAssert(unsigned char *p_ptucFile, const unsigned int p_uiLine)
{
  printf("Assert failed in file %p at line %d\n", p_ptucFile, p_uiLine);

#ifdef __DEBUG__
  while (1);
#else
    //stop le code
    exit(1);
#endif // __DEBUG__
}