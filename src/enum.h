/* enum.h - Church booleans and other enumerates */

#define ENUM_BOOLEAN(cond)	enum_of2[(cond) ? 0 : 1]
#define ENUM_ORDERING(diff)	enum_of3[(diff) < 0 ? 0 : (diff) > 0 ? 2 : 1]

extern NODE_ABSTRACTION *enum_of2[];
extern NODE_ABSTRACTION *enum_of3[];
