/* ast.h - abstract syntax tree, a.k.a. parse tree
**
** Note: this header file is used BOTH when compiling fblc and fblrt.a.
**
** AST_NODE_ID is defined when compiling fblc, NOT when compiling fblrt.a.
** AST_NAMES is defined when compiling fblc, and when compiling the debug
** version of fblrt.a; NOT when compiling the release version of fblrt.a.
*/

#ifdef AST_NODE_ID
#define OPTIONAL_NODE_ID   int node_id;
#define OPTIONAL_TYPE      /* TYPE *type; */
#define OPTIONAL_FUNC
#else
#define OPTIONAL_NODE_ID
#define OPTIONAL_TYPE
#define OPTIONAL_FUNC      void (*func)();
struct expr;
#endif

#ifdef AST_NAMES
#define OPTIONAL_NAME      const char *name;
#define OPTIONAL_PARENT    struct node_abstraction *parent;
#else
#define OPTIONAL_NAME
#define OPTIONAL_PARENT
#endif

/* Note: ATOM is not used in compile time, only in runtime */
typedef enum {
	NT_IDENT, NT_APPLY, NT_LAMBDA, NT_RLET,
	NT_PRIM, NT_ATOM, NT_INT, NT_DOUBLE
} NODETYPE;

/* nodetypes NT_IDENT, NT_APPLY, NT_LAMBDA, NT_RLET */
#define USES_CONTEXT(nodetype)	((nodetype) < NT_PRIM)

typedef struct define {
	OPTIONAL_NODE_ID
	struct define *next;
	struct node *body;
	OPTIONAL_NAME
} DEFINE;

typedef struct node {
	OPTIONAL_NODE_ID
	NODETYPE nodetype;
	OPTIONAL_PARENT		/* only if USES_CONTEXT(nodetype) is true */
	OPTIONAL_TYPE
} NODE;

typedef struct {
	OPTIONAL_NODE_ID
	NODETYPE nodetype;
	OPTIONAL_PARENT
	OPTIONAL_TYPE
	DEFINE *defined_by;
	int index;		/* like De Bruijn index, but base-0 */
	OPTIONAL_NAME
} NODE_IDENT;

typedef struct {
	OPTIONAL_NODE_ID
	NODETYPE nodetype;
	OPTIONAL_PARENT
	OPTIONAL_TYPE
	NODE *function, *argument;
} NODE_APPLY;

typedef struct node_abstraction {
	OPTIONAL_NODE_ID
	NODETYPE nodetype;
	OPTIONAL_PARENT
	DEFINE *define;
	OPTIONAL_TYPE
	NODE *body;
} NODE_ABSTRACTION;

typedef struct {
	OPTIONAL_NODE_ID
	NODETYPE nodetype;
	OPTIONAL_TYPE
	OPTIONAL_FUNC
	OPTIONAL_NAME
} NODE_PRIM;

typedef struct {
	OPTIONAL_NODE_ID
	NODETYPE nodetype;
	OPTIONAL_TYPE
	long value;
} NODE_INT;

typedef struct {
	OPTIONAL_NODE_ID
	NODETYPE nodetype;
	OPTIONAL_TYPE
	double value;
} NODE_DOUBLE;

#define AS_NODE(ptr)         ((NODE             *)(ptr))	/* upcast */

#define AS_PRIM(ptr)         ((NODE_PRIM        *)(ptr))	/* downcast */
#define AS_IDENT(ptr)        ((NODE_IDENT       *)(ptr))
#define AS_INT(ptr)          ((NODE_INT         *)(ptr))
#define AS_DOUBLE(ptr)       ((NODE_DOUBLE      *)(ptr))
#define AS_APPLY(ptr)        ((NODE_APPLY       *)(ptr))
#define AS_ABSTRACTION(ptr)  ((NODE_ABSTRACTION *)(ptr))

#define GET_NODETYPE(ptr)    (AS_NODE(ptr)->type)

#ifdef AST_NODE_ID
extern DEFINE *create_define(DEFINE *next, NODE *body, const char *name);
extern NODE *create_prim(const char *name);
extern NODE *create_ident(const char *name);
extern NODE *create_int(long value);
extern NODE *create_double(double value);
extern NODE *create_string(const char *s);
extern NODE *create_apply(NODE *function, NODE *argument);
extern NODE *create_lambda(NODE *body, const char *name);
extern NODE *create_rlet(NODE *body, DEFINE *define);
#else
#ifdef AST_NAMES
#define INIT_DEFINE(next, body, name)	{next, body, name}
#define INIT_IDENT(parent, index, name)	{NT_IDENT, parent, (DEFINE *)0, index, name}
#define INIT_NODE(nodetype, parent, x, y)	{nodetype, parent, x, y}
#else
#define INIT_DEFINE(next, body, name)	{next, body}
#define INIT_IDENT(parent, index, name)	{NT_IDENT, (DEFINE *)0, index}
#define INIT_NODE(nodetype, parent, x, y)	{nodetype, x, y}
#endif
#endif
