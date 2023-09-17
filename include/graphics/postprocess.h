#ifndef GUARD_POSTPROCESS_H
#define GUARD_POSTPROCESS_H

#include "types.h"

#define POST_PROCESS_BLOOM 1

void init_postprocessing(void);
void clean_postprocessing(void);

extern const u32 bloom(const u32 hdr_fbo);

#endif
