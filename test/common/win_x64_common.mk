#
# Copyright (c) 2017-2023, Intel Corporation
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#     * Redistributions of source code must retain the above copyright notice,
#       this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of Intel Corporation nor the names of its contributors
#       may be used to endorse or promote products derived from this software
#       without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

INSTNAME = intel-ipsec-mb

!if !defined(PREFIX)
PREFIX = C:\Program Files
!endif

!if exist("$(PREFIX)\$(INSTNAME)\libIPSec_MB.lib")
IPSECLIB = "$(PREFIX)\$(INSTNAME)\libIPSec_MB.lib"
INCDIR = -I"$(PREFIX)\$(INSTNAME)"
!else
!if !defined(LIB_DIR)
LIB_DIR = ..\..\lib
!endif
IPSECLIB = "$(LIB_DIR)\libIPSec_MB.lib"
INCDIR = -I$(LIB_DIR) -I..\include
!endif

!if !defined(DEBUG_OPT)
DEBUG_OPT = /Od
!endif

!ifdef DEBUG
DCFLAGS = $(DEBUG_OPT) /DDEBUG /Z7
DLFLAGS = /debug
!else
DCFLAGS = /O2 /Oi
DLFLAGS =
!endif

# compiler
CC = cl

# _CRT_SECURE_NO_WARNINGS disables warning C4996 about insecure snprintf() being used
CFLAGS = /nologo /D_CRT_SECURE_NO_WARNINGS $(DCFLAGS) /Y- /W3 /WX- /Gm- /fp:precise /EHsc $(EXTRA_CFLAGS) $(INCDIR) /std:c11

#linker
LNK = link
# XVALID_LFLAGS = /out:$(XVALID_APP).exe $(DLFLAGS)

AS = nasm
AFLAGS = -Werror -fwin64 -Xvc -DWIN_ABI

# dependency
!ifndef DEPTOOL
DEPTOOL = ..\..\mkdep.bat
!endif
DEPFLAGS = $(INCDIR)

