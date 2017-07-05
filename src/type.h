/* type.h - possible types */

#define MASK_FUNCTION	1
#define MASK_INT	2
#define MASK_DBL	4
#define MASK_ALL	7

typedef struct type
{
	int mask;
	struct type *t1, *t2;
}
TYPE;

extern TYPE type_int;
extern TYPE type_int_int;
extern TYPE type_int_int_int;

extern TYPE type_dbl;
extern TYPE type_dbl_dbl;
extern TYPE type_dbl_dbl_dbl;

extern TYPE type_any;
extern TYPE type_any_any;
extern TYPE type_any_any_any;

#define type_bool  type_any_any_any

extern TYPE type_int_bool;
extern TYPE type_int_int_bool;

extern TYPE type_dbl_bool;
extern TYPE type_dbl_dbl_bool;
