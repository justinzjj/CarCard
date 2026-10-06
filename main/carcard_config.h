#pragma once

/* Private overrides are optional and must never be tracked by Git. */
#if !defined(CARCARD_PUBLIC_BUILD) && defined(__has_include)
#if __has_include("carcard_local_config.h")
#include "carcard_local_config.h"
#endif
#endif

#ifndef CARCARD_R36_PLATE
#define CARCARD_R36_PLATE "R36 DEMO"
#endif
#ifndef CARCARD_POLO_PLATE
#define CARCARD_POLO_PLATE "POLO DEMO"
#endif
