#/*
# * COPYRIGHT 2014 SEAGATE LLC
# *
# * THIS DRAWING/DOCUMENT, ITS SPECIFICATIONS, AND THE DATA CONTAINED
# * HEREIN, ARE THE EXCLUSIVE PROPERTY OF SEAGATE LLC,
# * ISSUED IN STRICT CONFIDENCE AND SHALL NOT, WITHOUT
# * THE PRIOR WRITTEN PERMISSION OF SEAGATE TECHNOLOGY LIMITED,
# * BE REPRODUCED, COPIED, OR DISCLOSED TO A THIRD PARTY, OR
# * USED FOR ANY PURPOSE WHATSOEVER, OR STORED IN A RETRIEVAL SYSTEM
# * EXCEPT AS ALLOWED BY THE TERMS OF SEAGATE LICENSES AND AGREEMENTS.
# *
# * YOU SHOULD HAVE RECEIVED A COPY OF SEAGATE'S LICENSE ALONG WITH
# * THIS RELEASE. IF NOT PLEASE CONTACT A SEAGATE REPRESENTATIVE
# * http://www.xyratex.com/contact
# *
# * Original author:  Ganesan Umanesan <ganesan.umanesan@seagate.com>
# * Original creation date: 10-Jan-2017
#*/


#sudo
SUDO =
#or not
undefine SUDO

PROFILE = map --profile --no-mpi
undefine PROFILE
#files
FILE1 = './file1'
FILE2 = './file2'

#executables
EXE1 = c0cp
EXE2 = c0ct
EXE3 = c0rm
EXE4 = fgen

#archieve/node names 
TARF = m0trace_$(shell date +%Y%m%d-%H%M%S).tar.bz2
TARN = $(shell ls -la m0trace.* &> /dev/null | wc -l)
NODE = $(shell eval uname -n)

#c0cp parameters
#valid block sizes are: 4KB ~ 32MB
#4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 
#1048576, 2097152, 4194304, 8388608, 16777216, 33554432    
BSZ = 8192
CNT = 32

#compilar/linker options
LFLAGS += -lm -lpthread -lrt -lgf_complete -lyaml -luuid -lmero
CFLAGS += -D_REENTRANT -D_GNU_SOURCE -DM0_INTERNAL='' -DM0_EXTERN=extern
CFLAGS += -fno-common 
#CFLAGS += -Wall -Werror
CFLAGS +=  -Wno-attributes -fno-strict-aliasing 
CFLAGS += -fno-omit-frame-pointer -g -O2 -Wno-unused-but-set-variable 
CFLAGS += -rdynamic 

all: array 

array:
	gcc c0appz_op.c test.c -I/usr/include/mero $(CFLAGS) $(LFLAGS) -o test

array-lib:
	gcc $(CFLAGS) -fPIC -shared -o libclovisarray.so c0appz_op.c -I/usr/include/mero $(LFLAGS) 


test: array array-lib
	$(SUDO) ./test 


clean:
	rm -f test libclovisarray.so
