#include "timer.h"

#define APB1_FQ 84000000

void calculate_timer_values(uint32_t fq, uint32_t *p_out, uint32_t *ar_out) {
	for (uint32_t p = 0; p <= 0xFFFF; p++) {
		uint32_t ar = APB1_FQ / ((p + 1) * fq);
	
		if (ar < 2) continue;
		
		ar--;
		
		// Outside 16-bit range, try a different scalar.
		if (ar > 0xFFFF) continue;
		
		*p_out = p;
		*ar_out = ar;
		
		return;
	}
}