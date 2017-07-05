/* enum.c - Church booleans and other enumerates */

#include "ast.h"

static DEFINE def_x = INIT_DEFINE((DEFINE *)0, AS_NODE(0), "_x");
static DEFINE def_y = INIT_DEFINE((DEFINE *)0, AS_NODE(0), "_y");
static DEFINE def_z = INIT_DEFINE((DEFINE *)0, AS_NODE(0), "_z");

#ifdef AST_NAMES
/* forward declarations */
static NODE_ABSTRACTION lmb2_z_y, lmb2_yz_y;
static NODE_ABSTRACTION lmb2_z_z, lmb2_yz_z;
static NODE_ABSTRACTION lmb3_z_x, lmb3_yz_x, lmb3_xyz_x;
static NODE_ABSTRACTION lmb3_z_y, lmb3_yz_y, lmb3_xyz_y;
static NODE_ABSTRACTION lmb3_z_z, lmb3_yz_z, lmb3_xyz_z;
#endif

static NODE_IDENT idn2_y = INIT_IDENT(&lmb2_z_y, 1, "_y");
static NODE_IDENT idn2_z = INIT_IDENT(&lmb2_z_z, 0, "_z");

static NODE_ABSTRACTION lmb2_z_y = INIT_NODE(NT_LAMBDA, &lmb2_yz_y, &def_z, AS_NODE(&idn2_y));
static NODE_ABSTRACTION lmb2_z_z = INIT_NODE(NT_LAMBDA, &lmb2_yz_z, &def_z, AS_NODE(&idn2_z));

static NODE_ABSTRACTION lmb2_yz_y = INIT_NODE(NT_LAMBDA, (NODE_ABSTRACTION *)0, &def_y, AS_NODE(&lmb2_z_y));
static NODE_ABSTRACTION lmb2_yz_z = INIT_NODE(NT_LAMBDA, (NODE_ABSTRACTION *)0, &def_y, AS_NODE(&lmb2_z_z));

static NODE_IDENT idn3_x = INIT_IDENT(&lmb3_z_x, 2, "_x");
static NODE_IDENT idn3_y = INIT_IDENT(&lmb3_z_y, 1, "_y");
static NODE_IDENT idn3_z = INIT_IDENT(&lmb3_z_z, 0, "_z");

static NODE_ABSTRACTION lmb3_z_x = INIT_NODE(NT_LAMBDA, &lmb3_yz_x, &def_z, AS_NODE(&idn3_x));
static NODE_ABSTRACTION lmb3_z_y = INIT_NODE(NT_LAMBDA, &lmb3_yz_y, &def_z, AS_NODE(&idn3_y));
static NODE_ABSTRACTION lmb3_z_z = INIT_NODE(NT_LAMBDA, &lmb3_yz_z, &def_z, AS_NODE(&idn3_z));

static NODE_ABSTRACTION lmb3_yz_x = INIT_NODE(NT_LAMBDA, &lmb3_xyz_x, &def_y, AS_NODE(&lmb3_z_x));
static NODE_ABSTRACTION lmb3_yz_y = INIT_NODE(NT_LAMBDA, &lmb3_xyz_y, &def_y, AS_NODE(&lmb3_z_y));
static NODE_ABSTRACTION lmb3_yz_z = INIT_NODE(NT_LAMBDA, &lmb3_xyz_z, &def_y, AS_NODE(&lmb3_z_z));

static NODE_ABSTRACTION lmb3_xyz_x = INIT_NODE(NT_LAMBDA, (NODE_ABSTRACTION *)0, &def_x, AS_NODE(&lmb3_yz_x));
static NODE_ABSTRACTION lmb3_xyz_y = INIT_NODE(NT_LAMBDA, (NODE_ABSTRACTION *)0, &def_x, AS_NODE(&lmb3_yz_y));
static NODE_ABSTRACTION lmb3_xyz_z = INIT_NODE(NT_LAMBDA, (NODE_ABSTRACTION *)0, &def_x, AS_NODE(&lmb3_yz_z));

NODE_ABSTRACTION *enum_of2[] = { &lmb2_yz_y, &lmb2_yz_z };
NODE_ABSTRACTION *enum_of3[] = { &lmb3_xyz_x, &lmb3_xyz_y, &lmb3_xyz_z };
