/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Sun 31 May 2015 12:17:22 AM CST
 * Description: 
 ************************************************************************/

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <map>
#include <algorithm>
#include <assert.h>
#include "url_tools.h"

static const int SUCCESS = 0;
static const int ERROR_PARAM = -1;
static const int OPEN_FILE_FAILED = -2;
static const int FILE_FORMAT_ERROR = -3;
static const int ILLEGAL_DOMAIN = -2;
static const int TRUNK_BUFFER_SMALL = -3;

static const char* top_domain [] = {
    "orea.",
    "aisa.",
    "zib.",
    "tac.",
    "moc.",
    "pooc.",
    "ude.",
    "vog.",
    "ofni.",
    "tni.",
    "sboj.",
    "lim.",
    "ibom.",
    "muesum.",
    "eman.",
    "ten.",
    "gro.",
    "orp.",
    "let.",
    "levart.",
    "xxx.",
    0
};

static const char* top_country [] = {
    "ca.",
    "da.",
    "ea.",
    "fa.",
    "ga.",
    "ia.",
    "la.",
    "ma.",
    "na.",
    "oa.",
    "qa.",
    "ra.",
    "sa.",
    "ta.",
    "ua.",
    "wa.",
    "xa.",
    "za.",
    "ab.",
    "bb.",
    "db.",
    "eb.",
    "fb.",
    "gb.",
    "hb.",
    "ib.",
    "jb.",
    "mb.",
    "nb.",
    "ob.",
    "rb.",
    "sb.",
    "tb.",
    "vb.",
    "wb.",
    "yb.",
    "zb.",
    "ac.",
    "cc.",
    "dc.",
    "fc.",
    "gc.",
    "hc.",
    "ic.",
    "kc.",
    "lc.",
    "mc.",
    "nc.",
    "oc.",
    "rc.",
    "uc.",
    "vc.",
    "xc.",
    "yc.",
    "zc.",
    "ed.",
    "jd.",
    "kd.",
    "md.",
    "od.",
    "zd.",
    "ce.",
    "ee.",
    "ge.",
    "he.",
    "re.",
    "se.",
    "te.",
    "ue.",
    "if.",
    "jf.",
    "kf.",
    "mf.",
    "of.",
    "rf.",
    "ag.",
    "bg.",
    "dg.",
    "eg.",
    "fg.",
    "gg.",
    "hg.",
    "ig.",
    "lg.",
    "mg.",
    "ng.",
    "pg.",
    "qg.",
    "rg.",
    "sg.",
    "tg.",
    "ug.",
    "wg.",
    "yg.",
    "kh.",
    "mh.",
    "nh.",
    "rh.",
    "th.",
    "uh.",
    "di.",
    "ei.",
    "li.",
    "mi.",
    "ni.",
    "oi.",
    "qi.",
    "ri.",
    "si.",
    "ti.",
    "ej.",
    "mj.",
    "oj.",
    "pj.",
    "ek.",
    "gk.",
    "hk.",
    "ik.",
    "mk.",
    "nk.",
    "pk.",
    "rk.",
    "wk.",
    "yk.",
    "zk.",
    "al.",
    "bl.",
    "cl.",
    "il.",
    "kl.",
    "rl.",
    "sl.",
    "tl.",
    "ul.",
    "vl.",
    "yl.",
    "am.",
    "cm.",
    "dm.",
    "em.",
    "gm.",
    "hm.",
    "km.",
    "lm.",
    "mm.",
    "nm.",
    "om.",
    "pm.",
    "qm.",
    "rm.",
    "sm.",
    "tm.",
    "um.",
    "vm.",
    "wm.",
    "xm.",
    "ym.",
    "zm.",
    "an.",
    "cn.",
    "en.",
    "fn.",
    "gn.",
    "in.",
    "ln.",
    "on.",
    "pn.",
    "rn.",
    "un.",
    "zn.",
    "mo.",
    "ap.",
    "ep.",
    "fp.",
    "gp.",
    "hp.",
    "kp.",
    "lp.",
    "mp.",
    "np.",
    "rp.",
    "sp.",
    "tp.",
    "wp.",
    "yp.",
    "aq.",
    "er.",
    "or.",
    "sr.",
    "ur.",
    "wr.",
    "as.",
    "bs.",
    "cs.",
    "ds.",
    "es.",
    "gs.",
    "hs.",
    "is.",
    "js.",
    "ks.",
    "ls.",
    "ms.",
    "ns.",
    "os.",
    "rs.",
    "ts.",
    "us.",
    "vs.",
    "ys.",
    "zs.",
    "ct.",
    "dt.",
    "ft.",
    "gt.",
    "ht.",
    "jt.",
    "kt.",
    "lt.",
    "mt.",
    "nt.",
    "ot.",
    "pt.",
    "rt.",
    "tt.",
    "vt.",
    "wt.",
    "zt.",
    "au.",
    "gu.",
    "ku.",
    "mu.",
    "su.",
    "yu.",
    "zu.",
    "av.",
    "cv.",
    "ev.",
    "gv.",
    "iv.",
    "nv.",
    "uv.",
    "fw.",
    "sw.",
    "ey.",
    "ty.",
    "uy.",
    "ry.",
    "az.",
    "mz.",
    "wz.",
    0
};

static const char* general_2nd_domain [] = {
    "moc.",
    "ten.",
    "gro.",
    "vog.",
    "ude.",
    "oc.",
    0
};

static const char* cn_domain [] = {
    "nc.ca.",
    "nc.moc.",
    "nc.ude.",
    "nc.vog.",
    "nc.ten.",
    "nc.gro.",
    "nc.ha.",
    "nc.jb.",
    "nc.qc.",
    "nc.jf.",
    "nc.dg.",
    "nc.sg.",
    "nc.zg.",
    "nc.xg.",
    "nc.ah.",
    "nc.bh.",
    "nc.eh.",
    "nc.ih.",
    "nc.kh.", // .hk.cn
    "nc.lh.",
    "nc.nh.",
    "nc.lj.",
    "nc.sj.",
    "nc.xj.",
    "nc.nl.",
    "nc.mn.",
    "nc.xn.",
    "nc.hq.",
    "nc.cs.",
    "nc.ds.",
    "nc.hs.",
    "nc.ns.",
    "nc.xs.",
    "nc.jt.",
    "nc.wt.",
    "nc.jx.",
    "nc.zx.",
    "nc.ny.",
    "nc.jz.",
    0
};

static const char* tw_domain [] = {
    "wt.ude.",
    "wt.vog.",
    "wt.lim.",
    "wt.moc.",
    "wt.ten.",
    "wt.gro.",
    "wt.vdi.",
    "wt.emag.",
    "wt.zibe.",
    "wt.bulc.",
    0
};

static const char* hk_domain [] = {
    "kh.moc.",
    "kh.ude.",
    "kh.vog.",
    "kh.vdi.",
    "kh.ten.",
    "kh.gro.",
    0
};

static const char* uk_domain [] = {
    "ku.ca.",
    "ku.oc.",
    "ku.vog.",
    "ku.dtl.",
    "ku.em.",
    "ku.dom.",
    "ku.ten.",
    "ku.shn.",
    "ku.cin.",
    "ku.gro.",
    "ku.tnemailrap.",
    "ku.clp.",
    "ku.hcs.",
    0
};

static const char* jp_domain [] = {
    "pj.da.",
    "pj.oc.",
    "pj.de.",
    "pj.og.",
    "pj.rg.",
    "pj.gl.",
    "pj.en.",
    "pj.ro.",
    0,
};

static const char* kr_domain [] = {
    "rk.oc.",
    "rk.en.",
    "rk.ro.",
    "rk.er.",
    "rk.ep.",
    "rk.og.",
    "rk.lim.",
    "rk.ca.",
    "rk.sh.",
    "rk.sm.",
    "rk.se.",
    "rk.cs.",
    "rk.gk.",
    "rk.luoes.",
    "rk.nasub.",
    "rk.ugead.",
    "rk.noehcni.",
    "rk.ujgnawg.",
    "rk.noejead.",
    "rk.naslu.",
    "rk.iggnoeyg.",
    "rk.nowgnag.",
    "rk.kubgnuhc.",
    "rk.mangnuhc.",
    "rk.kubnoej.",
    "rk.mannoej.",
    "rk.kubgnoeyg.",
    "rk.mangnoeyg.",
    "rk.ujej.",
    0
};

static const char* il_domain [] = {
    "li.ca.",
    "li.oc.",
    "li.gro.",
    "li.ten.",
    "li.21k.",
    "li.vog.",
    "li.inum.",
    "li.fdi.",
    0
};

static const char* nz_domain [] = {
    "zn.ca.",
    "zn.oc.",
    "zn.keeg.",
    "zn.neg.",
    "zn.iroam.",
    "zn.ten.",
    "zn.gro.",
    "zn.irc.",
    "zn.tvog.",
    "zn.iwi.",
    "zn.tnemailrap.",
    "zn.lim.",
    0
};

static const char* th_domain [] = {
    "ht.ni.",
    "ht.oc.",
    0
};

struct domain_map_entry {
    const char* top_domain;
    const char** second_domain;
};

static domain_map_entry  domain_map [] = {
    {"nc.", cn_domain},
    {"wt.", tw_domain},
    {"kh.", hk_domain},
    {"ku.", uk_domain},
    {"pj.", jp_domain},
    {"rk.", kr_domain},
    {"li.", il_domain},
    {"zn.", nz_domain},
    {"ht.", th_domain},
    {0, 0}
};

// '\r' '\n'
static char LFCR[256] = {
    0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

//rfc2396
//define following chars:
// '$' '%' '&' '+' ',' '/' ':' ';' '=' '?' '@'
static char RESERVED[256] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,1,1,1,0,0,0,0,1,1,0,0,1,
    0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

//pchar = uchar | ":" | "@" | "&" | "=" | "+"
//add ' ' '"' '<' '>' 32 34 60 62
static char PCHAR[256] = {    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,
    1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

//pchar | "/"
//add ' ' '"' '<' '>' 32 34 60 62
static char PARAM[256] = {    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

//uchar | reserved
//add ' ' '"' '<' '>' 32 34 60 62
static char UCH_RES[256] = {    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

static char URL_DIGIT[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static unsigned char URL_LET_DIGIT[256]= {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

//LETTER DIGIT HYPHEN
static unsigned char URL_LET_DIG_HY[256]= {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,
    1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static char URL_UP_LET[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

//translate the HEX to DEC
//return the DEC
//A-F:65-70   a-f:97-102 ->10-15 '0'-'9':48-57 -> 0-9
static char TO_CHAR[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0,
    0,10,11,12,13,14,15,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,10,11,12,13,14,15,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

//pchar = uchar | ":" | "@" | "&" | "=" | "+"
//add ' ' '"' '<' '>' 32 34 60 62
static char URL_PCHAR[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,
    1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

//pchar | "/"
//add ' ' '"' '<' '>' 32 34 60 62
static char URL_PARAM[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

//uchar | reserved
//add ' ' '"' '<' '>' 32 34 60 62
static char URL_UCH_RES[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

int to_char(char hex[2])
{

    int r;
    int h[2];
    if((TO_CHAR[(unsigned char)hex[0]]==0 && hex[0]!=48) ||  (TO_CHAR[(unsigned char)hex[1]]==0 && hex[1]!=48))
        return 0;
    h[0] = TO_CHAR[(unsigned char)hex[0]];
    h[1] = TO_CHAR[(unsigned char)hex[1]];
    r = 16*h[0]+h[1];
    return r;
}

void replace_slash(char *path)
{
    char *p, *pend;

    pend = strchr(path, '?');
    if (pend == NULL) {
        pend = strchr(path, ';');
    }
    if (pend == NULL) {
        pend = path + strlen(path);
    }

    for (p = path; p != pend; p++) {
        if (*p == '\\')
            *p = '/';
    }
}

int delete_inter(char * str)
{
    if(NULL==str)
        return 0;
    size_t len = strcspn(str, "\r\n");

    char *p1 = str + len;
    char *p2 = str + len;

    while(*p2 !='\0') {
        if (LFCR[(unsigned char )*p2]) {
            p2++;

        } else *p1++ = *p2++;
    }
    *p1 = '\0';
    return p1-str;
}

int parse_url(const char *input, char *site,size_t site_size,
              char *port, size_t port_size,char *path, size_t max_path_size)
{
    char              tmp[MAX_URL_LEN] = {'\0'};
    char              *pin = tmp;
    char              *p,*q;
    char              *p_query;

    if(strlen(input) >= MAX_URL_LEN) {
        printf("fail, url[%s] is too long\n", input);
        return 0;
    }

    strcpy(tmp,input);
    delete_inter(tmp);

    if(strncasecmp(pin,"http://",7) == 0) {
        pin+=7;
    } else if (strncasecmp(pin,"https://",8) == 0) {
        pin+=8;
    }

    /*get path*/
    p = strchr(pin,'/');
    p_query = strchr(pin,'?');
    if(NULL==p) {
        p = p_query;
    }
    if ((NULL!=p_query) && (p_query<=p)) {
        if(path!=NULL) {
            path[0]='\0' ;
            if(strlen(p_query)<max_path_size-1) {
                strncpy(path,"/",2) ;
                strcat(path,p_query) ;
            } else
                return 0 ;
        }
        *p_query='\0' ;
    } else {
        if(p != NULL) {
            if (path != NULL) {
                path[0] = '\0';
                if(strlen(p) < max_path_size)
                    strcpy(path,p);
                else
                    return 0;
            }
            *p = '\0';
        } else {
            if (path != NULL) {
                strcpy(path,"/");
            }
        }
    }

    q = strchr(pin,':');
    /*get port*/

    if(q != NULL) {
        if (port != NULL) {
            port[0] = '\0';
            if(strlen(q) < port_size && atoi(q+1) > 0) {
                strcpy(port,q+1);
            } else
                return 0;
        }
        *q = '\0';
    } else {
        if (port != NULL)
            port[0] = '\0';

    }
    /*check if the default port*/
    if((port!=NULL)&&(strncmp(port,"80",3))==0) {
        port[0]=0;
    }
    /*get site*/
    if (site != NULL) {
        site[0] = '\0';
        if(pin[0] != '\0' && strlen(pin) < site_size) {
            strcpy(site,pin);
        } else {
            printf("parse_url: site name too long or empty url[%s]\n",pin);
            return 0;
        }
        // site最后一个不要是'.'
        while (strlen(site) > 0 && site[strlen(site)-1] == '.') {
            site[strlen(site)-1] = '\0';
        }
    }
    return 1;
}

uint64_t MurmurHash64A(const void * key, int len, unsigned int seed)
{
    const uint64_t m = 0xc6a4a7935bd1e995;
    const int r = 47;

    uint64_t h = seed ^ (len * m);

    const uint64_t * data = (const uint64_t *)key;
    const uint64_t * end = data + (len/8);

    while(data != end)
    {
        uint64_t k = *data++;

        k *= m; 
        k ^= k >> r; 
        k *= m; 
        h ^= k;
        h *= m; 
    }

    const unsigned char * data2 = (const unsigned char*)data;

    switch(len & 7)
    {
    case 7: h ^= uint64_t(data2[6]) << 48;
    case 6: h ^= uint64_t(data2[5]) << 40;
    case 5: h ^= uint64_t(data2[4]) << 32;
    case 4: h ^= uint64_t(data2[3]) << 24;
    case 3: h ^= uint64_t(data2[2]) << 16;
    case 2: h ^= uint64_t(data2[1]) << 8;
    case 1: h ^= uint64_t(data2[0]);
            h *= m;
    };

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}

uint64_t get_url_sign64(const char *url)
{
    char normalized_url[MAX_URL_LEN];
    normalized_url[0] = '\0';

    if (!(normalize_url(url, normalized_url, MAX_URL_LEN)))
    {
        strncpy(normalized_url, url, MAX_URL_LEN);
    }
    normalized_url[MAX_URL_LEN - 1] = '\0';
    return MurmurHash64A(normalized_url, strlen(normalized_url), 0);
}

static int space_escape_path(char * path,int max_path_len)
{
    char * pin = path ;
    int  pout = 0 ;
    char url_tmp[MAX_URL_LEN] ;
    while(*pin!='\0') {
        if(*pin!=' ') {
            if(pout>=max_path_len -1)//we have to judge here...
                return 0 ;
            url_tmp[pout++] = *pin++ ;
        } else { //space
            if(pout>=max_path_len-3)
                return 0 ;
            url_tmp[pout++]='%' ;
            url_tmp[pout++]='2' ;
            url_tmp[pout++]='0' ;
            pin++ ;
        }
    }
    if(pout>=max_path_len)
        return 0 ;
    url_tmp[pout]='\0' ;

    strncpy(path,url_tmp,pout+1) ;
    return 1 ;
}

int normalize_site(char *site)
{
    int dotcount = 0;
    char *pin = site;
    char *pout = site;
    if(!URL_LET_DIG_HY[(unsigned char)*pin]) //[0-9a-zA-Z\-]
        return 0; //must start with "0-9a-zA-Z\-"

    while(URL_LET_DIG_HY[(unsigned char)*pin] || *pin=='%') { // [0-9a-zA-Z\-]
        if(URL_UP_LET[(unsigned char)*pin]) //[A-Z]
            *pin += 32;
        else {
            if(*pin=='%') {
                if(pin[1]!='\0' && pin[2]!='\0') {
                    unsigned char escape_char = to_char(pin+1) ;
                    if(URL_LET_DIG_HY[escape_char]) {
                        if (URL_UP_LET[escape_char]) {
                            pin[2] = escape_char+32;
                        } else {
                            pin[2] = escape_char;
                        }
                        pin+=2 ;
                    } else {
                        return 0 ;
                    }
                } else {
                    return 0 ;
                }
            }
        }
        *pout++ = *pin++;
    }
    if (pin == site)
        return 0;

    pin --;
    if(!URL_LET_DIGIT[(unsigned char)*pin++]) //[0-9a-zA-Z]
        return 0; //not end with "-"
    while(*pin == '.') {
        dotcount++;
        *pout++=*pin ++;
        while(URL_LET_DIG_HY[(unsigned char)*pin] || (*pin=='%')) {
            if(URL_UP_LET[(unsigned char)*pin])
                *pin += 32;
            else {
                if(*pin=='%') {
                    if(pin[1]!='\0' && pin[2]!='\0') {
                        unsigned char escape_char = to_char(pin+1) ;
                        if(URL_LET_DIG_HY[escape_char]==1) {
                            if (URL_UP_LET[escape_char]) {
                                pin[2] = escape_char + 32;
                            } else {
                                pin[2] = escape_char;
                            }
                            pin+=2 ;
                        } else {
                            return 0 ;
                        }
                    } else {
                        return 0 ;
                    }
                }
            }
            *pout++=*pin++;
        }
        pin --;
        if(!URL_LET_DIGIT[(unsigned char)*pin++])
            return 0;
    }
    if(dotcount == 0) {
        return 0;
    }
    if (*pin != 0)
        return 0;
    *pout = '\0';
    return 1;
}

int normalize_port(char *port)
{
    char *pin = port;
    unsigned uport;
    int k = 0;

    if (*port == 0)
        return 1;

    while(URL_DIGIT[(unsigned char)*pin]) {
        k++;
        pin++;
        if(k>5)
            return 0;
    }
    if (*pin != 0)
        return 0;
    uport = atoi(port);
    if (uport <= 0 || uport > 65535)
        return 0;

    if (uport != 80)
        sprintf(port, "%d", uport);
    else
        port[0] = '\0';
    return 1;
}

static int single_path_nointer(char *path, int len)
{
    char *pin;
    int   pout;
    char  hex[2];
    char  tmp_hex[2];
    char  escape_char;
    int space_flag = 0;
    //len = strlen(path);
    pin = path;
    pout = 0;
    //check abs_path
    if(*pin == '/') {
        path[pout++] = *pin ++;
    }

    //path
    while(PCHAR[(unsigned char)*pin]) {
        tmp_hex[0] = '\0';
        if((*pin == '%') && (*(pin+1) != '\0') && (*(pin+2) !='\0')) {
            strncpy(hex,++pin,2);
            strncpy(tmp_hex,hex,2);
            pin ++;
            escape_char = to_char(hex);
            if(escape_char == 0) {
                pin--;
                path[pout++]='%';
                continue;
            }
            *pin = escape_char;
        }
        if(PCHAR[(unsigned char)*pin] &&
           (tmp_hex[0]=='\0' || !RESERVED[(unsigned char)*pin])) {
            if(*pin == ' ') {
                space_flag = 1;
            }
            path[pout++] = *pin++;
        } else {
            path[pout++]='%';
            strncpy(&path[pout],tmp_hex,2);
            pout=pout+2;
            pin++;
        }
    }

    while(*pin == '/') {
        path[pout++] = *pin ++;
        while(PCHAR[(unsigned char)*pin]) {
            tmp_hex[0] = '\0';
            if((*pin == '%') && (*(pin+1) != '\0') && (*(pin+2) !='\0')) {
                strncpy(hex,++pin,2);
                strncpy(tmp_hex,hex,2);
                pin ++;
                escape_char = to_char(hex);
                if(escape_char == 0) {
                    pin--;
                    path[pout++]='%';
                    continue;
                }
                *pin = escape_char;
            }

            if(PCHAR[(unsigned char)*pin] &&
               (tmp_hex[0]=='\0' || !RESERVED[(unsigned char)*pin])) {
                if(*pin == ' ') {
                    space_flag = 1;
                }
                path[pout++] = *pin++;
            } else {
                path[pout++]='%';
                strncpy(&path[pout],tmp_hex,2);
                pout=pout+2;
                pin++;
            }
        }
    }

    // ";" params  == *(";" param)
    while(*pin == ';') {
        path[pout++] = *pin ++;
        while(PARAM[(unsigned char)*pin]) {
            tmp_hex[0] = '\0';
            if((*pin == '%') && (*(pin+1) != '\0') && (*(pin+2) !='\0')) {
                strncpy(hex,++pin,2);
                strncpy(tmp_hex,hex,2);
                pin ++;
                escape_char = to_char(hex);
                if(escape_char == 0) {
                    pin--;
                    path[pout++]='%';
                    continue;
                }
                *pin = escape_char;
            }
            if(PARAM[(unsigned char)*pin] &&
               (tmp_hex[0]=='\0' || !RESERVED[(unsigned char)*pin])) {
                if(*pin == ' ') {
                    space_flag = 1;
                }
                path[pout++] = *pin++;
            } else {
                path[pout++]='%';
                strncpy(&path[pout],tmp_hex,2);
                pout=pout+2;
                pin++;
            }
        }
    }

    //"?" query
    while(*pin == '?') {
        path[pout++] = *pin ++;
        while(UCH_RES[(unsigned char)*pin]) {
            tmp_hex[0] = '\0';
            if((*pin == '%') && (*(pin+1) != '\0') && (*(pin+2) !='\0')) {
                strncpy(hex,++pin,2);
                strncpy(tmp_hex,hex,2);
                pin ++;
                escape_char = to_char(hex);
                if(escape_char == 0) {
                    pin--;
                    path[pout++]='%';
                    continue;
                }
                *pin = escape_char;
            }
            if(UCH_RES[(unsigned char)*pin] &&
               (tmp_hex[0]=='\0' || !RESERVED[(unsigned char)*pin])) {
                if(*pin == ' ') {
                    space_flag = 1;
                }
                path[pout++] = *pin++;
            } else {
                path[pout++]='%';
                strncpy(&path[pout],tmp_hex,2);
                pout=pout+2;
                pin++;
            }
        }
    }
    //if(*pin != 0)    
    if(*pin != 0 && *pin != '#')
        return 0;    
    path[pout] = 0;
    if (pout > len )
        return 0;
    if(space_flag == 1) {
        return space_escape_path(path,MAX_PATH_LEN);;
    } else {
        return 1;
    }
    //check abs_path over
}

void normalize_path(char *path)
{
    char *p, *q;
    char tmp[MAX_URL_LEN];
    char *pend;
    size_t len = 0;

    assert(*path == '/');
    len = strlen(path);
    assert(len < MAX_URL_LEN);

    replace_slash(path);

    strcpy(tmp, path);

    pend = strchr(tmp, '?');
    if (pend == NULL) {
        pend = strchr(tmp, ';');
    }
    if (pend == NULL) {
        pend = tmp + strlen(tmp);
    }

    p = tmp+1;
    q = path;
    while (p != pend) {
        /* normal */
        if (*q != '/') {
            q++;
            *q = *p++;
            continue;
        }

        /* /./ */
        if (strncmp(p, "./", 2)==0) {
            p += 2;
            continue;
        }

        /* /../ */
        if (strncmp(p, "../", 3)==0) {
            p += 3;
            if (q == path) {
                continue;
            }
            /* change q to prev '/' */
            q--;
            while (*q != '/')
                q--;
            continue;
        }

        /* // */
        if (*p=='/') {
            p++;
            continue;
        }

        /* "/." */
        if (strncmp(p, ".", pend-p)==0) {
            p++;
            continue;
        }

        /* "/.." */
        if (strncmp(p, "..", pend-p)==0) {
            p += 2;
            if (q == path) {
                continue;
            }
            /* change q to prev '/' */
            q--;
            while (*q != '/')
                q--;
            continue;
        }

        q++;
        *q = *p++;

    }
    q++;
    strcpy(q, p);
    return ;
}

int isnormalized_url(const char *url)
{
    const char *pin;
    char  port[6];
    int   k = 0;
    unsigned uport;
    int dotcount=0;

    if(strncasecmp(url,"http://",7) == 0 || strncasecmp(url,"https://",8) == 0)
        return 0; // should not start with "http[s]://"
    else
        pin = url;

    //check host name
    while(URL_LET_DIG_HY[(unsigned char)*pin]) { // [0-9a-zA-Z\-]
        if(URL_UP_LET[(unsigned char)*pin]) //[A-Z]
            return 0; // site should be lower case.
        pin ++;
    }
    pin --;
    if(!URL_LET_DIGIT[(unsigned char)*pin++]) //[0-9a-zA-Z]
        return 0; //not end with "-"
    while(*pin == '.') {
        dotcount++;
        pin ++;
        while(URL_LET_DIG_HY[(unsigned char)*pin]) {
            if(URL_UP_LET[(unsigned char)*pin])
                return 0; // site should be lower case.
            pin ++;
        }
        pin --;
        if(!URL_LET_DIGIT[(unsigned char)*pin++])
            return 0;
    }
    if(dotcount == 0) {
        return 0;
    }
    //check host name over

    //check port
    if(*pin == ':') {
        pin ++;
        if(*pin != '/') {
            while(URL_DIGIT[(unsigned char)*pin]) {
                port[k++] = *pin++;
                if(k>5)
                    return 0;
            }
            port[k] = 0;
            if(*pin != '/')
                return 0;

            uport = atoi(port);
            if (uport <= 0 || uport > 65535)
                return 0;

            if (uport == 80)
                return 0; // should omit port 80
        } else
            return 0;
    } else if( (*pin != '/'))
        return 0;
    //check port over

    //check abs_path
    if(*pin == '/') {
        pin ++;
        //path
        while(URL_PCHAR[(unsigned char)*pin]) {
            pin++;
        }
        while(*pin == '/') {
            pin ++;
            while(URL_PCHAR[(unsigned char)*pin]) {
                pin ++;
            }
        }


        // ";" params  == *(";" param)
        while(*pin == ';') {
            pin ++;
            while(URL_PARAM[(unsigned char)*pin]) {
                pin ++;
            }
        }

        //"?" query
        while(*pin == '?') {
            pin ++;
            while(URL_UCH_RES[(unsigned char)*pin]) {
                pin ++;
            }
        }
        if(*pin != 0)
            return 0;
        if ( pin-url >= MAX_URL_LEN )
            return 0;
        return 1;
    } else
        return 0;

    //check abs_path over
}

int normalize_url(const char* url, char* buf,size_t buf_size)
{
    char site[MAX_SITE_LEN];
    char port[MAX_PORT_LEN];
    char path[MAX_PATH_LEN];

    if (parse_url(url, site,MAX_SITE_LEN, port,MAX_PORT_LEN, path,MAX_PATH_LEN) == 0)
        return 0;

    if (normalize_site(site) == 0)
        return 0;
    if (normalize_port(port) == 0)
        return 0;
    if (single_path_nointer(path, MAX_PATH_LEN) == 0)
        return 0;
    normalize_path(path);

    if (port[0] == '\0') {
        snprintf(buf, buf_size, "%s%s", site, path);
    } else {
        snprintf(buf, buf_size, "%s:%s%s", site, port, path);
    }
    return 1;
}

///////////////////////////////////////////////////////////////////////////////////


class DomainDict
{
public:
    typedef std::vector< std::string > DomainList;
    typedef std::map< std::string, DomainList > DomainListMap;
private:
    DomainList top_domain_;
    DomainList top_country_;
    DomainList general_2nd_domain_;
    DomainListMap country_domain_map_;
public:
    void clear();
    void load_default();
    int load_file(const char* path,const char* file_name);
    void add_top_domain(const char* s);
    void add_top_country(const char* s);
    void add_general_2nd_domain(const char* s);
    void add_country_domain(const char* s,const char* w);
public:
    const DomainList& get_top_domain() const{
        return top_domain_;
    }
    const DomainList& get_top_country() const{
        return top_country_;
    }
    const DomainList& get_general_2nd_domain() const{
        return general_2nd_domain_;
    }
    const DomainListMap& get_country_domain_map() const{
        return country_domain_map_;
    }
private:
    bool readline(const std::string& str);
};

void DomainDict::add_top_domain(const char* s)
{
    std::string t(s);
    std::reverse(t.begin(), t.end());
    top_domain_.push_back(t);
}

void DomainDict::add_top_country(const char* s)
{
    std::string t(s);
    std::reverse(t.begin(), t.end());
    top_country_.push_back(t);
}

void DomainDict::add_general_2nd_domain(const char* s)
{
    std::string t(s);
    std::reverse(t.begin(), t.end());
    general_2nd_domain_.push_back(t);
}

void DomainDict::add_country_domain(const char* s, const char* w)
{
    std::string t1(s);
    std::reverse(t1.begin(), t1.end());
    std::string t2(w);
    t2 += s;
    std::reverse(t2.begin(), t2.end());
    DomainList& x = country_domain_map_[t1];
    x.push_back(t2);
}

void DomainDict::clear()
{
    top_domain_.clear();
    top_country_.clear();
    general_2nd_domain_.clear();
    country_domain_map_.clear();
}

void DomainDict::load_default()
{
    for(int i = 0; top_domain[i]; i++) {
        top_domain_.push_back(top_domain[i]);
    }
    for(int i = 0; top_country[i]; i++) {
        top_country_.push_back(top_country[i]);
    }
    for(int i = 0; general_2nd_domain[i]; i++) {
        general_2nd_domain_.push_back(general_2nd_domain[i]);
    }
    for(int i = 0; domain_map[i].top_domain; i++) {
        const char* x = domain_map[i].top_domain;
        for(int j = 0; domain_map[i].second_domain[j]; j++) {
            const char* y = domain_map[i].second_domain[j];
            DomainList& z = country_domain_map_[x];
            z.push_back(y);
        }
    }
}

// ============================
//         td .org
//         td .com
//         tc .cn
//         tc .uk
//         2nd .net
//         2nd .com
//         cd .cn .bj
//         cd .cn .jx

bool DomainDict::readline(const std::string& str)
{
    std::stringstream buf(str);
    std::string type;
    std::string s, s2;
    buf >> type;
    if(type == "td") {
        buf >> s;
        if(s.size()) {
            add_top_domain(s.c_str());
            return true;
        }
        return false;
    } else if(type == "tc") {
        buf >> s;
        if(s.size()) {
            add_top_country(s.c_str());
            return true;
        }
        return false;
    } else if(type == "2nd") {
        buf >> s;
        if(s.size()) {
            add_general_2nd_domain(s.c_str());
            return true;
        }
        return false;
    } else if(type == "cd") {
        buf >> s >> s2;
        if(s.size() && s2.size()) {
            add_country_domain(s.c_str(), s2.c_str());
            return true;
        }
        return false;
    } else if(type == "") {
        return true;
    } else if(type == "#") {
        return true;
    }
    return false;
}

int DomainDict::load_file(const char* path, const char* file_name)
{
    // check parameter
    if((path == NULL) ||
       (file_name == NULL) ||
       (file_name[0] == 0)) {
        return ERROR_PARAM;
    }
    std::string file(path);
    if(path[0] == 0) {
        file.append(file_name);
    } else {
        file.append("/").append(file_name);
    }
    std::ifstream in;
    in.open(file.c_str());
    // whether it's opened ok.
    if(!in.is_open()) {
        return OPEN_FILE_FAILED;
    }
    std::string buf;
    while(getline(in, buf)) {
        if(!readline(buf)) {
            in.close();
            return FILE_FORMAT_ERROR;
        }
    }
    in.close();
    return SUCCESS;
}

static bool is_legal_char2(char ch)
{
    if(isdigit(ch) || ch == '.') {
        return true;
    }
    return false;
}

static int guess_is_dotip2(const char* site)
{
    int dot_count = 0;
    while(*site) {
        if(!is_legal_char2(*site)) {
            return 0;
        }
        if(*site == '.') {
            dot_count++;
        }
        site++;
    }
    if(dot_count == 3) {
        return 1;
    }
    return 0;
}

static inline int get_trunk_from_domain(const char* domain,
                                        char* trunk,
                                        int trunk_size)
{
    int size = 0;
    while(domain[size] && domain[size] != '.') {
        size++;
    }
    if(size >= trunk_size) {
        return -1;
    }
    if(domain[size]) {
        memcpy(trunk, domain, size);
        trunk[size] = '\0';
    } else {
        trunk[0] = 0;
    }
    return 0;
}

static inline const char* rev_match(const char* str, int len, const char* pattern)
{
    while(len > 0 && *pattern != 0) {
        len--;
        if(str[len] != *pattern) {
            break;
        }
        pattern++;
    }
    if(*pattern == 0) {
        return str + len;
    } else {
        return 0;
    }
}

static inline const char* rev_dot(const char* str, const char* dot)
{
    dot--;
    while(dot > str) {
        if(*dot == '.') {
            return dot + 1;
        }
        dot--;
    }
    return 0;
}

static const char* get_last_two_parts(const char* url)
{
    if(url == 0) {
        return url;
    }
    int urllen = strlen(url);
    int dotcount = 0;
    for(int i = urllen - 1; i >= 0; --i) {
        if(*(url + i) == '.') {
            dotcount++;
        }
        if(dotcount == 2) {
            return (url + i + 1);
        }
    }
    return 0;
}

static const char* get_domain_from_site(const DomainDict* dict, const char* site)
{
    const char* result = NULL;
    if(site == 0)
        return 0;
    int sitelen = strlen(site);
    if(sitelen == 0) {
        return 0;
    }
    const DomainDict::DomainList& t_top_domain = dict->get_top_domain();
    for(size_t i = 0; i < t_top_domain.size(); i++) {
        const char* match;
        match = rev_match(site, sitelen, t_top_domain[i].c_str());
        if(match != 0) {
            result = rev_dot(site, match);
            if(result == 0) {
                return site;
            } else {
                return result;
            }
        }
    }
    const DomainDict::DomainListMap& t_domain_map = dict->get_country_domain_map();
    for(DomainDict::DomainListMap::const_iterator it = t_domain_map.begin();
        it != t_domain_map.end(); ++it) {
        const char* match;
        match = rev_match(site, sitelen, it->first.c_str());
        if(match != 0) {
            const DomainDict::DomainList& t_second_domain = it->second;
            for(size_t i = 0; i < t_second_domain.size(); i++) {
                const char* match2;
                match2 = rev_match(site, sitelen, t_second_domain[i].c_str());
                if(match2 != 0) {
                    result = rev_dot(site, match2);
                    if(result == 0) {
                        return site;
                    } else {
                        return result;
                    }
                }
            }
            goto next;
        }
    }
next:
    const DomainDict::DomainList& t_top_country = dict->get_top_country();
    const DomainDict::DomainList& t_general_2nd_domain = dict->get_general_2nd_domain();
    for(size_t i = 0; i < t_top_country.size(); i++) {
        const char* match;
        match = rev_match(site, sitelen, t_top_country[i].c_str());
        if(match != 0) {
            for(size_t j = 0; j < t_general_2nd_domain.size(); j++) {
                const char* match2;
                match2 = rev_match(site, match - site, t_general_2nd_domain[j].c_str());
                if(match2 != 0) {
                    result = rev_dot(site, match2);
                    if(result == 0) {
                        return site;
                    } else {
                        return result;
                    }
                }
            }
            result = rev_dot(site, match);
            if(result == 0) {
                return site;
            } else {
                return result;
            }
        }
    }
    return get_last_two_parts(site);
}

static int fetch_trunk_and_domain(const DomainDict* dict,
                                  const char* site,
                                  const char** domain,
                                  char* trunk,
                                  int trunk_size)
{
    if(guess_is_dotip2(site)) {
        *domain = site;
        if(trunk) {
            int ret = snprintf(trunk, trunk_size, "%s", site);
            if(ret < 0 || ret >= trunk_size) {
                return TRUNK_BUFFER_SMALL;
            }
        }
        return SUCCESS;
    }

    const char* tmp_domain = get_domain_from_site(dict, site);
    while(tmp_domain && *tmp_domain=='.'){
        tmp_domain++;
    }
    if(tmp_domain) {    
        *domain = tmp_domain;
        if(trunk) {
            if(get_trunk_from_domain(tmp_domain, trunk, trunk_size) != 0) {
                return TRUNK_BUFFER_SMALL;
            }
        }
        // OK.
        return SUCCESS;
    } else {
        return ILLEGAL_DOMAIN;
    }
}

const char* fetch_maindomain(const char* site, char* trunk, int trunk_size,
                             bool recoveryMode)
{
    const char* domain = 0;
    static DomainDict dict;
    static bool init = false;
    if(!init) {
        dict.load_default();
        init = true;
    }
    if(fetch_trunk_and_domain(&dict, site, &domain, trunk, trunk_size) != SUCCESS) {
        if(recoveryMode) {
            strncpy(trunk, site, trunk_size - 1);
            trunk[trunk_size-1] = 0;
            return site;
        }
        return NULL;
    }
    return domain;
}

bool fetch_url_maindomain(const char *url, char *maindomain, int len)
{
    char site[len];
    if (1 == parse_url(url, site, MAX_SITE_LEN, NULL, 0, NULL, 0)) {
        const char *domain = fetch_maindomain(site, NULL, 0, false);
        if (NULL != domain) {
            strncpy(maindomain, domain, MAX_SITE_LEN);
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}
