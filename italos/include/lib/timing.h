#ifndef TIMING_H
#define TIMING_H
/*** File timing.h - Provides functions for timing algorithms ***/
/*
 *   Shane Saunders
 */
/* This file provides a timing structure and related functions which can be
 * used when timing multiple algorithms.  These functions keep the code tidy
 * and make modification easier.  The main use is for summing up, dividing,
 * then reporting the average for each time measurement.  A single timer
 * is also provided for simple time measurements.
 */
#include <time.h>

/* This file allows the user to specify the clock function that clockval()
 * points to:
 *     (default)        clock()       - ANSI C clock() function.
 *     USE_PTIME        gethrvtime()  - High resolution time in nanoseconds
 *                                      when ptime utility is used.  (Solaris)
 *     USE_UCLOCK       uclock()      - More accurate than clock().
 *                                      (If available)
 */
/* Use a 1 to select at most one of the following. */
#define USE_PTIME 1
#define USE_UCLOCK 0


/*** Type definitions ***/

/* Define clockval_t used for clock values. */
#if USE_PTIME
typedef hrtime_t clockval_t;
#elif USE_UCLOCK
typedef uclock_t clockval_t;
#else
typedef clock_t clockval_t;
#endif

/* Structure for recording multiple clock tick measurements. */
typedef struct timing {
    int n;  /* Number of separate time measurements held. */
    clockval_t *totals;  /* Array for holding each time measurement. */
} timing_t;


/*** Global Constants ***/

/* Pointer to clock function. */
extern clockval_t (*getclock)(void);

/* Number of clock ticks per second. */
extern const clockval_t CLOCK_DIV;


/*** Single Timer Functions ***/

/* Reset the total accumulated time on the timer to zero. */
void timer_reset();

/* Start the timer. */
void timer_start();

/* Stop the timer.  This does not reset the total accumulated time on the
 * timer.  The elapsed time is taken as the time since the last call to either
 * timer_start() or timer_stop() and is added to the total accumulated time
 * whenever timer_stop() is called.  The elapsed time is returned in
 * clock_val_t units.
 */
clockval_t timer_stop();

/* Return the total accumulated time on the timer in clockval_t units. */
clockval_t timer_total();

/* Print the total accumulated time divided by div in milliseconds.  The
 * divisor div allows the user to print out an average or scale the
 * measurement to some other units.  The value is printed according to the
 * printf() format specifier in the string pointed to by fmt.
 */
void timer_print(char *fmt, int div);


/*** Functions for Recording Multiple Time Measurements ***/

/* Allocate a timing structure for storing n measurements.  Returns a pointer
 * to the timing structure.  Other functions identify a particular measurement
 * using an index parameter, i, which ranges between 0 and n-1.  The user is
 * responsible for passing a valid index, since no bounds checking is
 * performed.
 */
timing_t *timing_alloc(int n);

/* Free the timing structure pointed to by t. */
void timing_free(timing_t *t);

/* Reset all measurements in the timing structure pointed to by t to zero. */
void timing_reset(timing_t *t);

/* Reset measurement i in the timing structure pointed to by t to zero. */
void timing_reset1(timing_t *t, int i);

/* Start the timer (same as timer_start). */
void timing_start();

/* Stop the timer and add the elapsed time to the total accumulated time on
 * measurement i in the timing structure pointed to by t.  The elapsed time is
 * taken as the time since the last call to either timing_start() or
 * timing_stop() and is added to the total accumulated time whenever
 * timing_stop() is called.  The elapsed time is returned in clock_val_t units.
 */
clockval_t timing_stop(timing_t *t, int i);

/* Return the total accumulated time on measurement i in the timing structure
 * pointed to by t.
 */
clockval_t timing_total(timing_t *t, int i);

/* Print all measurements in the timing structure pointed to by t.  This prints
 * the total accumulated time divided by div in milliseconds.  The divisor div
 * allows the user to print out an average or scale the measurement to some
 * other units. The time values are printed in index order according to the
 * printf() format specifier in the string pointed to by fmt.  The format
 * specifier supplied must be appropriate for a floating point value, and is
 * allowed to include separator characters.  For example "\t%.2f" will print
 * values to two decimal places, with each separated by a tab character.
 */
void timing_print(timing_t *t, char *fmt, int div);

/* For measurement i in the timing structure pointed to by t, print the total
 * accumulated time divided by div in milliseconds.  The divisor div allows the
 * user to print out an average or scale the measurement to some other units.
 * The time value is printed according to the printf() format specifier in the
 * string pointed to by fmt.  The format specifier supplied must be appropriate
 * for a floating point value.
 */
void timing_print1(timing_t *t, int i, char *fmt, int div);

/*** Specialised Timing Functions ***/

/* timing_sub() - Specialised function used for subtracting a fixed amount from
 * all time measurements in the timing structure pointed to by t.  This
 * subtracts the elapsed time from the total accumulated time for each
 * measurement.  The elapsed time is taken as the last call to either
 * timing_start(), timing_stop, or timing_sub().
 *
 * This is useful for discarding overhead from time from measurements.  After
 * calling timing_start(), a dummy calculation involving only overhead can be
 * run and then timing_sub() called.
 */
clockval_t timing_sub(timing_t *t);

#endif /* TIMING_H */
