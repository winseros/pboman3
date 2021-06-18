#pragma once

#include <QDebug>

#define __NUM_ARGS_10TH(A10, A9, A8, A7, A6, A5, A4, A3, A2, A1, A0, ...) A0
#define __NUM_ARGS(...) __NUM_ARGS_10TH(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

#define __LOG_IMPL_A1(A1) A1
#define __LOG_IMPL_A2(A1, A2) __LOG_IMPL_A1(A1) << A2
#define __LOG_IMPL_A3(A1, A2, A3) __LOG_IMPL_A2(A1, A2) << A3
#define __LOG_IMPL_A4(A1, A2, A3, A4) __LOG_IMPL_A3(A1, A2, A3) << A4
#define __LOG_IMPL_A5(A1, A2, A3, A4, A5) __LOG_IMPL_A4(A1, A2, A3, A4) << A5
#define __LOG_IMPL_A6(A1, A2, A3, A4, A5, A6) __LOG_IMPL_A5(A1, A2, A3, A4, A5) << A6
#define __LOG_IMPL_A7(A1, A2, A3, A4, A5, A6, A7) __LOG_IMPL_A6(A1, A2, A3, A4, A5, A6) << A7
#define __LOG_IMPL_A8(A1, A2, A3, A4, A5, A6, A7, A8) __LOG_IMPL_A7(A1, A2, A3, A4, A5, A6, A7) << A8
#define __LOG_IMPL_A9(A1, A2, A3, A4, A5, A6, A7, A8, A9) __LOG_IMPL_A8(A1, A2, A3, A4, A5, A6, A7, A8) << A9
#define __LOG_IMPL_A10(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) __LOG_IMPL_A9(A1, A2, A3, A4, A5, A6, A7, A8, A9) << A10

#define __LOG_IMPL3(TARGET, N, ...) TARGET << __LOG_IMPL_A##N(__VA_ARGS__)
#define __LOG_IMPL2(TARGET, N, ...) __LOG_IMPL3(TARGET, N, __VA_ARGS__)
#define __LOG_IMPL(LOG, LEVEL, ...) __LOG_IMPL2(LOG.LEVEL(), __NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

#define LOGGER(CLASS, ...) __LOG_IMPL(QMessageLogger(CLASS, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC), __VA_ARGS__);

/*
USE THIS CODE FOR MACRO DEBUGGING
#define __DEBUG_STRING2(x) #x
#define DEBUG_STRING(x) __DEBUG_STRING2(x)
#pragma message(DEBUG_STRING(LOGGER("Main", debug, "Hello!")))
*/
