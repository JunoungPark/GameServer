#pragma once

#define CRASH(cause)			\
{								\
	int* p = 0;					\
	__analysis_assume(p != 0);	\
	*p = 1;						\
}								\

#define CONDITION_CRASH(condition)	\
{									\
	if (!condition)					\
	{								\
		CRASH("CRASH");             \
	}								\
}									\
