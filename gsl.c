/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.  If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright 1997 - July 2008 CWI, August 2008 - 2017 MonetDB B.V.
 */

/*
 *  A. de Rijke, B. Scheers
 * The gsl module
 * The gsl module contains wrappers for functions in
 * gsl.
 */

#include "monetdb_config.h"
#include "mal.h"
#include "mal_exception.h"
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>

#ifdef WIN32
#define gsl_export extern __declspec(dllexport)
#else
#define gsl_export extern
#endif

#define CONSTANTFUN(LABEL,NAME)						\
  static str								\
  gsl ## NAME(dbl *retval)						\
  {									\
    *retval = dbl_nil;							\
    *retval = NAME;							\
    return MAL_SUCCEED;							\
  }									\
  gsl_export str							\
  gsl_ ## NAME(dbl *retval)						\
  {									\
    return gsl ## NAME(retval);						\
  }
#define FUNCTION0(NAME)							\
  static str								\
  gsl ## NAME(dbl *retval)						\
  {									\
    *retval = dbl_nil;							\
    *retval = NAME();							\
    return MAL_SUCCEED;							\
  }									\
  gsl_export str							\
  gsl_ ## NAME(dbl *retval)						\
  {									\
    return gsl ## NAME(retval);						\
  }
#define FUNCTION1(NAME,TYPE,ARG)					\
  static str								\
  gsl ## NAME(dbl *retval, TYPE ARG)					\
  {									\
    *retval = dbl_nil;							\
    if (ARG == TYPE ## _nil)						\
      throw(MAL, "gsl." #NAME, "Wrong value for " #ARG);		\
    *retval = NAME(ARG);						\
    return MAL_SUCCEED;							\
  }									\
  gsl_export str							\
  gsl_ ## NAME(dbl *retval, TYPE *ARG)					\
  {									\
    return gsl ## NAME(retval, *ARG);					\
  }									\
  static str								\
  batgsl ## NAME(bat *retval, bat ARG)					\
  {									\
    BAT *b_, *bn;							\
    BATiter bi;								\
    BUN p_, q_;								\
    dbl r_;								\
    if ((b_ = BATdescriptor(ARG)) == NULL) {				\
      throw(MAL, "batgsl." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    bi = bat_iterator(b_);						\
    bn = COLnew(b_->hseqbase, TYPE_dbl, BATcount(b_), TRANSIENT);	\
    if (bn == NULL) {							\
      BBPunfix(b_->batCacheid);						\
      throw(MAL, "batgsl." # NAME, MAL_MALLOC_FAIL);			\
    }									\
    BATloop(b_, p_, q_) {						\
      dbl d_ = *(dbl *) BUNtail(bi, p_);				\
      if (d_ == dbl_nil) {						\
	BBPunfix(b_->batCacheid);					\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for " # ARG);		\
      }									\
      r_ = NAME(d_);							\
      if (BUNappend(bn, &r_, FALSE) != GDK_SUCCEED) {			\
	BBPunfix(b_->batCacheid);					\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, GDK_EXCEPTION);			\
      }									\
    }									\
    *retval = bn->batCacheid;						\
    BBPkeepref(bn->batCacheid);						\
    BBPunfix(b_->batCacheid);						\
    return MAL_SUCCEED;							\
  }									\
  gsl_export str							\
  bat_gsl_ ## NAME(bat *retval, bat *ARG)				\
  {									\
    return batgsl ## NAME(retval, *ARG);				\
  }
#define FUNCTION2(NAME,TYPE1,ARG1,TYPE2,ARG2)				\
  static str								\
  gsl ## NAME(dbl *retval, TYPE1 ARG1, TYPE2 ARG2)			\
  {									\
    *retval = dbl_nil;							\
    if (ARG1 == TYPE1 ## _nil)						\
      throw(MAL, "gsl." #NAME, "Wrong value for " #ARG1);		\
    if (ARG2 == TYPE2 ## _nil)						\
      throw(MAL, "gsl." #NAME, "Wrong value for " #ARG2);		\
    *retval = NAME(ARG1,ARG2);						\
    return MAL_SUCCEED;							\
  }									\
  gsl_export str							\
  gsl_ ## NAME(dbl *retval, TYPE1 *ARG1, TYPE2 *ARG2)			\
  {									\
    return gsl ## NAME(retval, *ARG1, *ARG2);				\
  }									\
  static str								\
  batgsl ## NAME(bat *retval, bat ARG1, TYPE2 ARG2)			\
  {									\
    BAT *b_, *bn;							\
    BATiter bi;								\
    BUN p_, q_;								\
    dbl r_;								\
    if ((b_ = BATdescriptor(ARG1)) == NULL) {				\
      throw(MAL, "batgsl." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    if (ARG2 == TYPE2 ## _nil)						\
      throw(MAL, "batgsl." #NAME, "Wrong value for " #ARG2);		\
    bi = bat_iterator(b_);						\
    bn = COLnew(b_->hseqbase, TYPE_dbl, BATcount(b_), TRANSIENT);	\
    if (bn == NULL) {							\
      BBPunfix(b_->batCacheid);						\
      throw(MAL, "batgsl." # NAME, MAL_MALLOC_FAIL);			\
    }									\
    BATloop(b_, p_, q_) {						\
      dbl d_ = *(dbl *) BUNtail(bi, p_);				\
      if (d_ == dbl_nil) {						\
	BBPunfix(b_->batCacheid);					\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for " # ARG1);	\
      }									\
      r_ = NAME(d_,ARG2);						\
      if (BUNappend(bn, &r_, FALSE) != GDK_SUCCEED) {			\
	BBPunfix(b_->batCacheid);					\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, GDK_EXCEPTION);			\
      }									\
    }									\
    *retval = bn->batCacheid;						\
    BBPkeepref(bn->batCacheid);						\
    BBPunfix(b_->batCacheid);						\
    return MAL_SUCCEED;							\
  }									\
  gsl_export str							\
    bat_gsl_ ## NAME(bat *retval, bat *ARG1, TYPE2 *ARG2)		\
  {									\
    return batgsl ## NAME(retval, *ARG1, *ARG2);			\
  }									\
  static str								\
  batgslbats ## NAME(bat *retval, bat ARG1, bat ARG2)			\
  {									\
    BAT * arg[2];							\
    BAT *bn;								\
    dbl r_;								\
    TYPE1* valp1;							\
    TYPE2* valp2;							\
    size_t cnt = 0, i, j, kk, ncol=2;					\
    if (((arg[j=0] = BATdescriptor(ARG1)) == NULL) ||			\
	((arg[j=1] = BATdescriptor(ARG2)) == NULL)) {			\
      for (kk = 0; kk < j; kk++)					\
	BBPunfix(arg[kk]->batCacheid);					\
      throw(MAL, "batgsl." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    bn = COLnew(arg[0]->hseqbase, TYPE_dbl, cnt = BATcount(arg[0]), TRANSIENT); \
    if (bn == NULL) {							\
      for (kk=0; kk<ncol; kk++)						\
	BBPunfix(arg[kk]->batCacheid);					\
      throw(MAL, "batgsl." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    valp1 = (TYPE1 *) Tloc(arg[0], 0);					\
    valp2 = (TYPE2 *) Tloc(arg[1], 0);					\
    for (i = 0; i < cnt; i++) {						\
      if (valp1[i] == TYPE1 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for argument" # ARG1); \
      }									\
      if (valp2[i] == TYPE2 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for argument" # ARG2); \
      }									\
      r_ = NAME(valp1[i], valp2[i]);					\
      if (BUNappend(bn, &r_, FALSE) != GDK_SUCCEED) {			\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, GDK_EXCEPTION);			\
      }									\
    }									\
    BBPkeepref(*retval = bn->batCacheid);				\
    for (kk=0; kk<ncol; kk++)						\
      BBPunfix(arg[kk]->batCacheid);					\
    return MAL_SUCCEED;							\
  }									\
  gsl_export str							\
  bat_gsl_bats_ ## NAME(bat *retval, bat *ARG1, bat *ARG2)		\
  {									\
    return batgslbats ## NAME(retval, *ARG1, *ARG2);			\
  }									
#define FUNCTION3(NAME,TYPE1,ARG1,TYPE2,ARG2,TYPE3,ARG3)		\
  static str								\
  gsl ## NAME(dbl *retval, TYPE1 ARG1, TYPE2 ARG2, TYPE3 ARG3)		\
  {									\
    *retval = dbl_nil;							\
    if (ARG1 == TYPE1 ## _nil)						\
      throw(MAL, "gsl." #NAME, "Wrong value for " #ARG1);		\
    if (ARG2 == TYPE2 ## _nil)						\
      throw(MAL, "gsl." #NAME, "Wrong value for " #ARG2);		\
    if (ARG3 == TYPE3 ## _nil)						\
      throw(MAL, "gsl." #NAME, "Wrong value for " #ARG3);		\
    *retval = NAME(ARG1,ARG2,ARG3);					\
    return MAL_SUCCEED;							\
  }									\
  gsl_export str							\
  gsl_ ## NAME(dbl *retval, TYPE1 *ARG1, TYPE2 *ARG2, TYPE3 *ARG3)	\
  {									\
    return gsl ## NAME(retval, *ARG1, *ARG2, *ARG3);			\
  }									\
  static str								\
  batgsl ## NAME(bat *retval, bat ARG1, TYPE2 ARG2, TYPE3 ARG3)		\
  {									\
    BAT *b_, *bn;							\
    BATiter bi;								\
    BUN p_, q_;								\
    dbl r_;								\
    if ((b_ = BATdescriptor(ARG1)) == NULL) {				\
      throw(MAL, "batgsl." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    if (ARG2 == TYPE2 ## _nil)						\
      throw(MAL, "batgsl." #NAME, "Wrong value for " #ARG2);		\
    if (ARG3 == TYPE3 ## _nil)						\
      throw(MAL, "batgsl." #NAME, "Wrong value for " #ARG3);		\
    bi = bat_iterator(b_);						\
    bn = COLnew(b_->hseqbase, TYPE_dbl, BATcount(b_), TRANSIENT);	\
    if (bn == NULL) {							\
      BBPunfix(b_->batCacheid);						\
      throw(MAL, "batgsl." # NAME, MAL_MALLOC_FAIL);			\
    }									\
    BATloop(b_, p_, q_) {						\
      dbl d_ = *(dbl *) BUNtail(bi, p_);				\
      if (d_ == dbl_nil) {						\
	BBPunfix(b_->batCacheid);					\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for " # ARG1);	\
      }									\
      r_ = NAME(d_,ARG2,ARG3);						\
      if (BUNappend(bn, &r_, FALSE) != GDK_SUCCEED) {			\
	BBPunfix(b_->batCacheid);					\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, GDK_EXCEPTION);			\
      }									\
    }									\
    *retval = bn->batCacheid;						\
    BBPkeepref(bn->batCacheid);						\
    BBPunfix(b_->batCacheid);						\
    return MAL_SUCCEED;							\
  }									\
  gsl_export str							\
  bat_gsl_ ## NAME(bat *retval, bat *ARG1, TYPE2 *ARG2, TYPE3 *ARG3)	\
  {									\
    return batgsl ## NAME(retval, *ARG1, *ARG2, *ARG3);			\
  }									\
  static str								\
  batgslbats ## NAME(bat *retval, bat ARG1, bat ARG2, bat ARG3)		\
  {									\
    BAT * arg[3];							\
    BAT *bn;								\
    dbl r_;								\
    TYPE1* valp1;							\
    TYPE2* valp2;							\
    TYPE3* valp3;							\
    size_t cnt = 0, i, j, kk, ncol=3;					\
    if (((arg[j=0] = BATdescriptor(ARG1)) == NULL) ||			\
	((arg[j=1] = BATdescriptor(ARG2)) == NULL) ||			\
	((arg[j=2] = BATdescriptor(ARG3)) == NULL)) {			\
      for (kk = 0; kk < j; kk++)					\
	BBPunfix(arg[kk]->batCacheid);					\
      throw(MAL, "batgsl." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    bn = COLnew(arg[0]->hseqbase, TYPE_dbl, cnt = BATcount(arg[0]), TRANSIENT); \
    if (bn == NULL) {							\
      for (kk=0; kk<ncol; kk++)						\
	BBPunfix(arg[kk]->batCacheid);					\
      throw(MAL, "batgsl." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    valp1 = (TYPE1 *) Tloc(arg[0], 0);					\
    valp2 = (TYPE2 *) Tloc(arg[1], 0);					\
    valp3 = (TYPE3 *) Tloc(arg[2], 0);					\
    for (i = 0; i < cnt; i++) {						\
      if (valp1[i] == TYPE1 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for argument" # ARG1); \
      }									\
      if (valp2[i] == TYPE2 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for argument" # ARG2); \
      }									\
      if (valp3[i] == TYPE3 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for argument" # ARG3); \
      }									\
      r_ = NAME(valp1[i], valp2[i], valp3[i]);				\
      if (BUNappend(bn, &r_, FALSE) != GDK_SUCCEED) {			\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, GDK_EXCEPTION);			\
      }									\
    }									\
    BBPkeepref(*retval = bn->batCacheid);				\
    for (kk=0; kk<ncol; kk++)						\
      BBPunfix(arg[kk]->batCacheid);					\
    return MAL_SUCCEED;							\
  }									\
  gsl_export str							\
  bat_gsl_bats_ ## NAME(bat *retval, bat *ARG1, bat *ARG2, bat *ARG3)	\
  {									\
    return batgslbats ## NAME(retval, *ARG1, *ARG2, *ARG3);		\
  }									
#define FUNCTION4(NAME,TYPE1,ARG1,TYPE2,ARG2,TYPE3,ARG3,TYPE4,ARG4)	\
  static str								\
  gsl ## NAME(dbl *retval, TYPE1 ARG1, TYPE2 ARG2, TYPE3 ARG3, TYPE4 ARG4) \
  {									\
    *retval = dbl_nil;							\
    if (ARG1 == TYPE1 ## _nil)						\
      throw(MAL, "gsl." #NAME, "Wrong value for " #ARG1);		\
    if (ARG2 == TYPE2 ## _nil)						\
      throw(MAL, "gsl." #NAME, "Wrong value for " #ARG2);		\
    if (ARG3 == TYPE3 ## _nil)						\
      throw(MAL, "gsl." #NAME, "Wrong value for " #ARG3);		\
    if (ARG4 == TYPE4 ## _nil)						\
      throw(MAL, "gsl." #NAME, "Wrong value for " #ARG4);		\
    *retval = NAME(ARG1,ARG2,ARG3,ARG4);				\
    return MAL_SUCCEED;							\
  }									\
  gsl_export str							\
  gsl_ ## NAME(dbl *retval, TYPE1 *ARG1, TYPE2 *ARG2, TYPE3 *ARG3, TYPE4 *ARG4) \
  {									\
    return gsl ## NAME(retval, *ARG1, *ARG2, *ARG3, *ARG4);		\
  }									\
  static str								\
  batgsl ## NAME(bat *retval, bat ARG1, TYPE2 ARG2, TYPE3 ARG3, TYPE4 ARG4) \
  {									\
    BAT *b_, *bn;							\
    BATiter bi;								\
    BUN p_, q_;								\
    dbl r_;								\
    if ((b_ = BATdescriptor(ARG1)) == NULL) {				\
      throw(MAL, "batgsl." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    if (ARG2 == TYPE2 ## _nil)						\
      throw(MAL, "batgsl." #NAME, "Wrong value for " #ARG2);		\
    if (ARG3 == TYPE3 ## _nil)						\
      throw(MAL, "batgsl." #NAME, "Wrong value for " #ARG3);		\
    if (ARG4 == TYPE4 ## _nil)						\
      throw(MAL, "batgsl." #NAME, "Wrong value for " #ARG4);		\
    bi = bat_iterator(b_);						\
    bn = COLnew(b_->hseqbase, TYPE_dbl, BATcount(b_), TRANSIENT);	\
    if (bn == NULL) {							\
      BBPunfix(b_->batCacheid);						\
      throw(MAL, "batgsl." # NAME, MAL_MALLOC_FAIL);			\
    }									\
    BATloop(b_, p_, q_) {						\
      dbl d_ = *(dbl *) BUNtail(bi, p_);				\
      if (d_ == dbl_nil) {						\
	BBPunfix(b_->batCacheid);					\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for " # ARG1);	\
      }									\
      r_ = NAME(d_,ARG2,ARG3,ARG4);					\
      if (BUNappend(bn, &r_, FALSE) != GDK_SUCCEED) {			\
	BBPunfix(b_->batCacheid);					\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, GDK_EXCEPTION);			\
      }									\
    }									\
    *retval = bn->batCacheid;						\
    BBPkeepref(bn->batCacheid);						\
    BBPunfix(b_->batCacheid);						\
    return MAL_SUCCEED;							\
  }									\
  gsl_export str							\
  bat_gsl_ ## NAME(bat *retval, bat *ARG1, TYPE2 *ARG2, TYPE3 *ARG3, TYPE4 *ARG4) \
  {									\
    return batgsl ## NAME(retval, *ARG1, *ARG2, *ARG3, *ARG4);		\
  }									\
  static str								\
  batgslbats ## NAME(bat *retval, bat ARG1, bat ARG2, bat ARG3, bat ARG4) \
  {									\
    BAT * arg[4];							\
    BAT *bn;								\
    dbl r_;								\
    TYPE1* valp1;							\
    TYPE2* valp2;							\
    TYPE3* valp3;							\
    TYPE4* valp4;							\
    size_t cnt = 0, i, j, kk, ncol=4;					\
    if (((arg[j=0] = BATdescriptor(ARG1)) == NULL) ||			\
	((arg[j=1] = BATdescriptor(ARG2)) == NULL) ||			\
	((arg[j=2] = BATdescriptor(ARG3)) == NULL) ||			\
	((arg[j=3] = BATdescriptor(ARG4)) == NULL)) {			\
      for (kk = 0; kk < j; kk++)					\
	BBPunfix(arg[kk]->batCacheid);					\
      throw(MAL, "batgsl." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    bn = COLnew(arg[0]->hseqbase, TYPE_dbl, cnt = BATcount(arg[0]), TRANSIENT); \
    if (bn == NULL) {							\
      for (kk=0; kk<ncol; kk++)						\
	BBPunfix(arg[kk]->batCacheid);					\
      throw(MAL, "batgsl." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    valp1 = (TYPE1 *) Tloc(arg[0], 0);					\
    valp2 = (TYPE2 *) Tloc(arg[1], 0);					\
    valp3 = (TYPE3 *) Tloc(arg[2], 0);					\
    valp4 = (TYPE4 *) Tloc(arg[3], 0);					\
    for (i = 0; i < cnt; i++) {						\
      if (valp1[i] == TYPE1 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for argument" # ARG1); \
      }									\
      if (valp2[i] == TYPE2 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for argument" # ARG2); \
      }									\
      if (valp3[i] == TYPE3 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for argument" # ARG3); \
      }									\
      if (valp4[i] == TYPE4 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for argument" # ARG4); \
      }									\
      r_ = NAME(valp1[i], valp2[i], valp3[i], valp4[i]); \
      if (BUNappend(bn, &r_, FALSE) != GDK_SUCCEED) {			\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, GDK_EXCEPTION);			\
      }									\
    }									\
    BBPkeepref(*retval = bn->batCacheid);				\
    for (kk=0; kk<ncol; kk++)						\
      BBPunfix(arg[kk]->batCacheid);					\
    return MAL_SUCCEED;							\
  }									\
  gsl_export str							\
  bat_gsl_bats_ ## NAME(bat *retval, bat *ARG1, bat *ARG2, bat *ARG3, bat *ARG4) \
  {									\
    return batgslbats ## NAME(retval, *ARG1, *ARG2, *ARG3, *ARG4);	\
  }									
#define FUNCTION5(NAME,TYPE1,ARG1,TYPE2,ARG2,TYPE3,ARG3,TYPE4,ARG4,TYPE5,ARG5) \
  static str								\
  gsl ## NAME(dbl *retval, TYPE1 ARG1, TYPE2 ARG2, TYPE3 ARG3, TYPE4 ARG4, TYPE5 ARG5) \
  {									\
    *retval = dbl_nil;							\
    if (ARG1 == TYPE1 ## _nil)						\
      throw(MAL, "gsl." #NAME, "Wrong value for " #ARG1);		\
    if (ARG2 == TYPE2 ## _nil)						\
      throw(MAL, "gsl." #NAME, "Wrong value for " #ARG2);		\
    if (ARG3 == TYPE3 ## _nil)						\
      throw(MAL, "gsl." #NAME, "Wrong value for " #ARG3);		\
    if (ARG4 == TYPE4 ## _nil)						\
      throw(MAL, "gsl." #NAME, "Wrong value for " #ARG4);		\
    if (ARG5 == TYPE5 ## _nil)						\
      throw(MAL, "gsl." #NAME, "Wrong value for " #ARG5);		\
    *retval = NAME(ARG1,ARG2,ARG3,ARG4,ARG5);				\
    return MAL_SUCCEED;							\
  }									\
  gsl_export str							\
  gsl_ ## NAME(dbl *retval, TYPE1 *ARG1, TYPE2 *ARG2, TYPE3 *ARG3, TYPE4 *ARG4, TYPE5 *ARG5) \
  {									\
    return gsl ## NAME(retval, *ARG1, *ARG2, *ARG3, *ARG4, *ARG5);	\
  }									\
  static str								\
  batgsl ## NAME(bat *retval, bat ARG1, TYPE2 ARG2, TYPE3 ARG3, TYPE4 ARG4, TYPE4 ARG5) \
  {									\
    BAT *b_, *bn;							\
    BATiter bi;								\
    BUN p_, q_;								\
    dbl r_;								\
    if ((b_ = BATdescriptor(ARG1)) == NULL) {				\
      throw(MAL, "batgsl." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    if (ARG2 == TYPE2 ## _nil)						\
      throw(MAL, "batgsl." #NAME, "Wrong value for " #ARG2);		\
    if (ARG3 == TYPE3 ## _nil)						\
      throw(MAL, "batgsl." #NAME, "Wrong value for " #ARG3);		\
    if (ARG4 == TYPE4 ## _nil)						\
      throw(MAL, "batgsl." #NAME, "Wrong value for " #ARG4);		\
    if (ARG5 == TYPE5 ## _nil)						\
      throw(MAL, "batgsl." #NAME, "Wrong value for " #ARG5);		\
    bi = bat_iterator(b_);						\
    bn = COLnew(b_->hseqbase, TYPE_dbl, BATcount(b_), TRANSIENT);	\
    if (bn == NULL) {							\
      BBPunfix(b_->batCacheid);						\
      throw(MAL, "batgsl." # NAME, MAL_MALLOC_FAIL);			\
    }									\
    BATloop(b_, p_, q_) {						\
      dbl d_ = *(dbl *) BUNtail(bi, p_);				\
      if (d_ == dbl_nil) {						\
	BBPunfix(b_->batCacheid);					\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for " # ARG1);	\
      }									\
      r_ = NAME(d_,ARG2,ARG3,ARG4,ARG5);				\
      if (BUNappend(bn, &r_, FALSE) != GDK_SUCCEED) {			\
	BBPunfix(b_->batCacheid);					\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, GDK_EXCEPTION);			\
      }									\
    }									\
    *retval = bn->batCacheid;						\
    BBPkeepref(bn->batCacheid);						\
    BBPunfix(b_->batCacheid);						\
    return MAL_SUCCEED;							\
  }									\
  gsl_export str							\
    bat_gsl_ ## NAME(bat *retval, bat *ARG1, TYPE2 *ARG2, TYPE3 *ARG3, TYPE4 *ARG4, TYPE5 *ARG5) \
  {									\
    return batgsl ## NAME(retval, *ARG1, *ARG2, *ARG3, *ARG4, *ARG5);	\
  }									\
  static str								\
  batgslbats ## NAME(bat *retval, bat ARG1, bat ARG2, bat ARG3, bat ARG4, bat ARG5) \
  {									\
    BAT * arg[5];							\
    BAT *bn;								\
    dbl r_;								\
    TYPE1* valp1;							\
    TYPE2* valp2;							\
    TYPE3* valp3;							\
    TYPE4* valp4;							\
    TYPE5* valp5;							\
    size_t cnt = 0, i, j, kk, ncol=5;					\
    if (((arg[j=0] = BATdescriptor(ARG1)) == NULL) ||			\
	((arg[j=1] = BATdescriptor(ARG2)) == NULL) ||			\
	((arg[j=2] = BATdescriptor(ARG3)) == NULL) ||			\
	((arg[j=3] = BATdescriptor(ARG4)) == NULL) ||			\
	((arg[j=4] = BATdescriptor(ARG5)) == NULL)) {			\
      for (kk = 0; kk < j; kk++)					\
	BBPunfix(arg[kk]->batCacheid);					\
      throw(MAL, "batgsl." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    bn = COLnew(arg[0]->hseqbase, TYPE_dbl, cnt = BATcount(arg[0]), TRANSIENT); \
    if (bn == NULL) {							\
      for (kk=0; kk<ncol; kk++)						\
	BBPunfix(arg[kk]->batCacheid);					\
      throw(MAL, "batgsl." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    valp1 = (TYPE1 *) Tloc(arg[0], 0);					\
    valp2 = (TYPE2 *) Tloc(arg[1], 0);					\
    valp3 = (TYPE3 *) Tloc(arg[2], 0);					\
    valp4 = (TYPE4 *) Tloc(arg[3], 0);					\
    valp5 = (TYPE5 *) Tloc(arg[4], 0);					\
    for (i = 0; i < cnt; i++) {						\
      if (valp1[i] == TYPE1 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for argument" # ARG1); \
      }									\
      if (valp2[i] == TYPE2 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for argument" # ARG2); \
      }									\
      if (valp3[i] == TYPE3 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for argument" # ARG3); \
      }									\
      if (valp4[i] == TYPE4 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for argument" # ARG4); \
      }									\
      if (valp5[i] == TYPE5 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for argument" # ARG5); \
      }									\
      r_ = NAME(valp1[i], valp2[i], valp3[i], valp4[i], valp5[i]);	\
      if (BUNappend(bn, &r_, FALSE) != GDK_SUCCEED) {			\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, GDK_EXCEPTION);			\
      }									\
    }									\
    BBPkeepref(*retval = bn->batCacheid);				\
    for (kk=0; kk<ncol; kk++)						\
      BBPunfix(arg[kk]->batCacheid);					\
    return MAL_SUCCEED;							\
  }									\
  gsl_export str							\
  bat_gsl_bats_ ## NAME(bat *retval, bat *ARG1, bat *ARG2, bat *ARG3, bat *ARG4, bat *ARG5) \
  {									\
    return batgslbats ## NAME(retval, *ARG1, *ARG2, *ARG3, *ARG4, *ARG5); \
  }									
#define FUNCTION6(NAME,TYPE1,ARG1,TYPE2,ARG2,TYPE3,ARG3,TYPE4,ARG4,TYPE5,ARG5,TYPE6,ARG6) \
  static str								\
  gsl ## NAME(dbl *retval, TYPE1 ARG1, TYPE2 ARG2, TYPE3 ARG3, TYPE4 ARG4, TYPE5 ARG5, TYPE6 ARG6) \
  {									\
    *retval = dbl_nil;							\
    if (ARG1 == TYPE1 ## _nil)						\
      throw(MAL, "gsl." #NAME, "Wrong value for " #ARG1);		\
    if (ARG2 == TYPE2 ## _nil)						\
      throw(MAL, "gsl." #NAME, "Wrong value for " #ARG2);		\
    if (ARG3 == TYPE3 ## _nil)						\
      throw(MAL, "gsl." #NAME, "Wrong value for " #ARG3);		\
    if (ARG4 == TYPE4 ## _nil)						\
      throw(MAL, "gsl." #NAME, "Wrong value for " #ARG4);		\
    if (ARG5 == TYPE5 ## _nil)						\
      throw(MAL, "gsl." #NAME, "Wrong value for " #ARG5);		\
    if (ARG6 == TYPE6 ## _nil)						\
      throw(MAL, "gsl." #NAME, "Wrong value for " #ARG6);		\
    *retval = NAME(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6);			\
    return MAL_SUCCEED;							\
  }									\
  gsl_export str							\
  gsl_ ## NAME(dbl *retval, TYPE1 *ARG1, TYPE2 *ARG2, TYPE3 *ARG3, TYPE4 *ARG4, TYPE5 *ARG5, TYPE6 *ARG6) \
  {									\
    return gsl ## NAME(retval, *ARG1, *ARG2, *ARG3, *ARG4, *ARG5, *ARG6); \
  }									\
  static str								\
  batgsl ## NAME(bat *retval, bat ARG1, TYPE2 ARG2, TYPE3 ARG3, TYPE4 ARG4, TYPE5 ARG5, TYPE6 ARG6) \
  {									\
    BAT *b_, *bn;							\
    BATiter bi;								\
    BUN p_, q_;								\
    dbl r_;								\
    if ((b_ = BATdescriptor(ARG1)) == NULL) {				\
      throw(MAL, "batgsl." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    if (ARG2 == TYPE2 ## _nil)						\
      throw(MAL, "batgsl." #NAME, "Wrong value for " #ARG2);		\
    if (ARG3 == TYPE3 ## _nil)						\
      throw(MAL, "batgsl." #NAME, "Wrong value for " #ARG3);		\
    if (ARG4 == TYPE4 ## _nil)						\
      throw(MAL, "batgsl." #NAME, "Wrong value for " #ARG4);		\
    if (ARG5 == TYPE5 ## _nil)						\
      throw(MAL, "batgsl." #NAME, "Wrong value for " #ARG5);		\
    if (ARG6 == TYPE6 ## _nil)						\
      throw(MAL, "batgsl." #NAME, "Wrong value for " #ARG6);		\
    bi = bat_iterator(b_);						\
    bn = COLnew(b_->hseqbase, TYPE_dbl, BATcount(b_), TRANSIENT);	\
    if (bn == NULL) {							\
      BBPunfix(b_->batCacheid);						\
      throw(MAL, "batgsl." # NAME, MAL_MALLOC_FAIL);			\
    }									\
    BATloop(b_, p_, q_) {						\
      dbl d_ = *(dbl *) BUNtail(bi, p_);				\
      if (d_ == dbl_nil) {						\
	BBPunfix(b_->batCacheid);					\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for " # ARG1);	\
      }									\
      r_ = NAME(d_,ARG2,ARG3,ARG4,ARG5,ARG6);				\
      if (BUNappend(bn, &r_, FALSE) != GDK_SUCCEED) {			\
	BBPunfix(b_->batCacheid);					\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, GDK_EXCEPTION);			\
      }									\
    }									\
    *retval = bn->batCacheid;						\
    BBPkeepref(bn->batCacheid);						\
    BBPunfix(b_->batCacheid);						\
    return MAL_SUCCEED;							\
  }									\
  gsl_export str							\
  bat_gsl_ ## NAME(bat *retval, bat *ARG1, TYPE2 *ARG2, TYPE3 *ARG3, TYPE4 *ARG4, TYPE5 *ARG5, TYPE6 *ARG6) \
  {									\
    return batgsl ## NAME(retval, *ARG1, *ARG2, *ARG3, *ARG4, *ARG5, *ARG6); \
  }									\
  static str								\
  batgslbats ## NAME(bat *retval, bat ARG1, bat ARG2, bat ARG3, bat ARG4, bat ARG5, bat ARG6) \
  {									\
    BAT * arg[6];							\
    BAT *bn;								\
    dbl r_;								\
    TYPE1* valp1;							\
    TYPE2* valp2;							\
    TYPE3* valp3;							\
    TYPE4* valp4;							\
    TYPE5* valp5;							\
    TYPE6* valp6;							\
    size_t cnt = 0, i, j, kk, ncol=6;					\
    if (((arg[j=0] = BATdescriptor(ARG1)) == NULL) ||			\
	((arg[j=1] = BATdescriptor(ARG2)) == NULL) ||			\
	((arg[j=2] = BATdescriptor(ARG3)) == NULL) ||			\
	((arg[j=3] = BATdescriptor(ARG4)) == NULL) ||			\
	((arg[j=4] = BATdescriptor(ARG5)) == NULL) ||			\
	((arg[j=5] = BATdescriptor(ARG6)) == NULL)) {			\
      for (kk = 0; kk < j; kk++)					\
	BBPunfix(arg[kk]->batCacheid);					\
      throw(MAL, "batgsl." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    bn = COLnew(arg[0]->hseqbase, TYPE_dbl, cnt = BATcount(arg[0]), TRANSIENT); \
    if (bn == NULL) {							\
      for (kk=0; kk<ncol; kk++)						\
	BBPunfix(arg[kk]->batCacheid);					\
      throw(MAL, "batgsl." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    valp1 = (TYPE1 *) Tloc(arg[0], 0);					\
    valp2 = (TYPE2 *) Tloc(arg[1], 0);					\
    valp3 = (TYPE3 *) Tloc(arg[2], 0);					\
    valp4 = (TYPE4 *) Tloc(arg[3], 0);					\
    valp5 = (TYPE5 *) Tloc(arg[4], 0);					\
    valp6 = (TYPE6 *) Tloc(arg[5], 0);					\
    for (i = 0; i < cnt; i++) {						\
      if (valp1[i] == TYPE1 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for argument" # ARG1); \
      }									\
      if (valp2[i] == TYPE2 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for argument" # ARG2); \
      }									\
      if (valp3[i] == TYPE3 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for argument" # ARG3); \
      }									\
      if (valp4[i] == TYPE4 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for argument" # ARG4); \
      }									\
      if (valp5[i] == TYPE5 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for argument" # ARG5); \
      }									\
      if (valp6[i] == TYPE6 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, "Wrong value for argument" # ARG6); \
      }									\
      r_ = NAME(valp1[i], valp2[i], valp3[i], valp4[i], valp5[i], valp6[i]); \
      if (BUNappend(bn, &r_, FALSE) != GDK_SUCCEED) {			\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batgsl." # NAME, GDK_EXCEPTION);			\
      }									\
    }									\
    BBPkeepref(*retval = bn->batCacheid);				\
    for (kk=0; kk<ncol; kk++)						\
      BBPunfix(arg[kk]->batCacheid);					\
    return MAL_SUCCEED;							\
  }									\
  gsl_export str							\
  bat_gsl_bats_ ## NAME(bat *retval, bat *ARG1, bat *ARG2, bat *ARG3, bat *ARG4, bat *ARG5, bat *ARG6) \
  {									\
    return batgslbats ## NAME(retval, *ARG1, *ARG2, *ARG3, *ARG4, *ARG5, *ARG6); \
  }									

#define DIST1FUNCTION(NAME,ARG)						\
  FUNCTION2(gsl_ran_ ## NAME ## _pdf,dbl,x,dbl,ARG)			\
  FUNCTION2(gsl_cdf_ ## NAME ## _P,dbl,x,dbl,ARG)			\
  FUNCTION2(gsl_cdf_ ## NAME ## _Q,dbl,x,dbl,ARG)			\
  FUNCTION2(gsl_cdf_ ## NAME ## _Pinv,dbl,x,dbl,ARG)			\
  FUNCTION2(gsl_cdf_ ## NAME ## _Qinv,dbl,x,dbl,ARG)

#define DIST2FUNCTION(NAME,ARG1,ARG2)					\
  FUNCTION3(gsl_ran_ ## NAME ## _pdf,dbl,x,dbl,ARG1,dbl,ARG2)		\
  FUNCTION3(gsl_cdf_ ## NAME ## _P,dbl,x,dbl,ARG1,dbl,ARG2)		\
  FUNCTION3(gsl_cdf_ ## NAME ## _Q,dbl,x,dbl,ARG1,dbl,ARG2)		\
  FUNCTION3(gsl_cdf_ ## NAME ## _Pinv,dbl,x,dbl,ARG1,dbl,ARG2)		\
  FUNCTION3(gsl_cdf_ ## NAME ## _Qinv,dbl,x,dbl,ARG1,dbl,ARG2)

#define DIST3FUNCTION(NAME,ARG1,ARG2,ARG3)				\
  FUNCTION4(gsl_ran_ ## NAME ## _pdf,dbl,x,dbl,ARG1,dbl,ARG2,dbl,ARG3)	\
  FUNCTION4(gsl_cdf_ ## NAME ## _P,dbl,x,dbl,ARG1,dbl,ARG2,dbl,ARG3)	\
  FUNCTION4(gsl_cdf_ ## NAME ## _Q,dbl,x,dbl,ARG1,dbl,ARG2,dbl,ARG3)	\
  FUNCTION4(gsl_cdf_ ## NAME ## _Pinv,dbl,x,dbl,ARG1,dbl,ARG2,dbl,ARG3)	\
  FUNCTION4(gsl_cdf_ ## NAME ## _Qinv,dbl,x,dbl,ARG1,dbl,ARG2,dbl,ARG3)

DIST1FUNCTION(chisq,nu)
DIST2FUNCTION(gamma,a,b)

double poisson_ci(double x, int boundary, double conflevel) {
  double alpha = (1.0-conflevel)/2.0;
  if (x==0 && boundary==1) return 0.0;
  else if (boundary==1) return gsl_cdf_gamma_Pinv(alpha, x, 1.0);
  else return gsl_cdf_gamma_Qinv(alpha,x + 1,1.0);
}
FUNCTION3(poisson_ci,dbl,x,int,boundary,dbl,conflevel)


#undef DIST1FUNCTION
#undef DIST2FUNCTION
#undef DIST3FUNCTION
#undef CONSTANTFUN
#undef FUNCTION0
#undef FUNCTION1
#undef FUNCTION2
#undef FUNCTION3
#undef FUNCTION4
#undef FUNCTION5
#undef FUNCTION6
#undef gsl_export
