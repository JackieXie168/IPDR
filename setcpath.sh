#* Copyright (C) 2002, The IPDR Organization, all rights reserved.
#* The use and distribution of this software is governed by the terms of
#* the license agreement which can be found in the file LICENSE.TXT at
#* the top of this source tree.
#*
#* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
#* ANY KIND, either express or implied.

# *****************************************************
# * File  Name           : setcpath.sh                *
# * Description          : Shell Script for setting   *
# *                        LD_LIBRARY_PATH            *
# *                        in C on Unix Platform   *
# * Author               : Infosys Tech Ltd           *
# * Modification History :                            *
# *---------------------------------------------------*
# * Date       Name             Change/Description    *
# *---------------------------------------------------*
# * 01/18/02  Infosys           Created               *
# *****************************************************

IPDRROOTPATH=`pwd`
export IPDRROOTPATH

OS=`uname`
export OS
# Updating the Environment Variable LD_LIBRARY_PATH
if test $OS = "SunOS" -o $OS = "sunOS"
then LD_LIBRARY_PATH=/usr/local/lib:$IPDRROOTPATH/libs:;
export LD_LIBRARY_PATH
elif test $OS = "Linux" -o $OS = "linux"
then LD_LIBRARY_PATH=/usr/local/lib:$IPDRROOTPATH/libs:;
export LD_LIBRARY_PATH
elif test $OS = "HP-UX" -o $OS = "hp-ux"
then SH_LIBRARY_PATH=/usr/local/lib:$IPDRROOTPATH/libs:;
export SH_LIBRARY_PATH
else
echo "Wrong option. Execute script again."
fi

export OS
# End of Shell Script File setcpath.sh

