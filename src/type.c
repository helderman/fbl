/* type.c - possible types */

#include "type.h"

TYPE type_int = {MASK_INT, NULL, NULL};
TYPE type_int_int = {MASK_FUNC, &type_int, &type_int};
TYPE type_int_int_int = {MASK_FUNC, &type_int, &type_int_int};

TYPE type_dbl = {MASK_DBL, NULL, NULL};
TYPE type_dbl_dbl = {MASK_FUNC, &type_dbl, &type_dbl};
TYPE type_dbl_dbl_dbl = {MASK_FUNC, &type_dbl, &type_dbl_dbl};

TYPE type_any = {MASK_ALL, NULL, NULL};
TYPE type_any_any = {MASK_FUNC, &type_any, &type_any};
TYPE type_any_any_any = {MASK_FUNC, &type_any, &type_any_any};

TYPE type_int_bool = {MASK_FUNC, &type_int, &type_bool};
TYPE type_int_int_bool = {MASK_FUNC, &type_int, &type_int_bool};

TYPE type_dbl_bool = {MASK_FUNC, &type_dbl, &type_bool};
TYPE type_dbl_dbl_bool = {MASK_FUNC, &type_dbl, &type_dbl_bool};
