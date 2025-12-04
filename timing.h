#ifndef TIMING_H
#define TIMING_H

long long now_ms(void);          // wall-clock time in ms
void       timing_set_start(void);
long long  since_start_ms(void); // ms since program start

#endif // TIMING_H
