/* trace.h - trace evaluation process */

#ifdef TRACE

extern int trace_count_expr;
extern int trace_count_context;
extern int trace_count_stack;
extern int trace_count_max_stack;
extern int trace_count_gc;

extern void trace_output_counts(void);
extern void trace_exit(long exitcode);

#endif
