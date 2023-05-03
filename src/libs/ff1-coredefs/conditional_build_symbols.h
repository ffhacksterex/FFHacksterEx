#pragma once

#ifdef USE_DEPRCATION
	#pragma message(" *** Using [[deprecated]] in " __FILE__)
	#define mark_deprecated [[deprecated]]
#else
	#define mark_deprecated
#endif
