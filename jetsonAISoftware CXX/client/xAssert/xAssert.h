//
// Created by christophe on 6/30/24.
//

#ifndef XASSERT_H
#define XASSERT_H

/*
 * @brief Assert macro
 */
#define X_ASSERT(expr) ((expr) ? (void)0 : xAssert((uint8_t *)__FILE__, __LINE__));

/*
 * @brief function call when an assert is triggered
 *
 * @param file: file name
 * @param line: line number
 * @return void
 */
void xAssert(unsigned char *p_ptucFile, const unsigned int p_uiLine);

#endif //XASSERT_H
