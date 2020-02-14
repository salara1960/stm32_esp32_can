#!/bin/bash
#
#    Compile SW4STM32 project from comand line (in folder Build)
#
#            set ENV
#
CrossCompilerPath="/home/${USER}/Ac6/SystemWorkbench/plugins/fr.ac6.mcu.externaltools.arm-none.linux64_1.17.0.201812190825/tools/compiler/bin"
set_envCC="CROSS_COMPILE=arm-none-eabi-"
set_envPATH="PATH=$PATH:$CrossCompilerPath"
#
#            set compile options, it is not yet known where to get (from project) for automatic mode
#
###########################       for stm32f407         ###########################################
#
#sdkPATH="/home/alarm/STM32Cube/Repository/STM32Cube_FW_F4_V1.24.2/"
#aCFLAG="-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16"
#
###########################       for stm32f103         ###########################################
#
sdkPATH=""
aCFLAG="-mcpu=cortex-m3 -mthumb -mfloat-abi=soft"
#
###################################################################################################
#
cCFLAGop="-Os -g3 -Wall -ffunction-sections -c -fmessage-length=0"
SPECS="-specs=nano.specs"
#
############################     Default variables       ##########################################
#
SRC=Src
INC=Inc
MKDIR=Build
ASMDIR=startup
OBJLIST=object.list
SDKSRC_FILE=sdklink.src
SDKHDR_FILE=sdklink.hdr
SDKDEF_FILE=sdklink.def
SDK=Sdk
localINC=Inc
startDir=${PWD}
pName=stm
varCC="CROSS_COMPILE"
varCCvalue="arm-"

inHdrFile=".prop.xml"
inDefFile=".prop.xml"
inSrcFile=".project"
#
##############################        Any functions         #######################################
#
checkDir() {
    if [ ! -d "$1" ]; then
        mkdir $1
    fi
}
# $1 contain $2 ?
strstr() {
    [ "${1#*$2*}" = "$1" ] && return 1
    return 0
}
#
checkFile() {
    if [ -f "$1" ] ; then
        FSIZE=`stat -c%s $1`
        if [ "$FSIZE" -eq 0 ] ; then
            echo "ERROR : file '$1' have size = $FSIZE. Bye."
            exit 1
        fi
    else
        exit 1
    fi
}
#
delFile() {
    if [ -f "$1" ] ; then
        rm -f $1
    fi
}
#
###############################################################################################
#
DIR=$startDir
PROJECT=$pName
if [ "$#" -eq 3 ]; then
    DIR=$2
    PROJECT=$3
else
    if [ "$#" -eq 2 ]; then
        DIR=$2
    fi
fi
if [ "$DIR" = "pwd" ] ; then
    DIR=$startDir
fi
#
################################################################################################
#
case $1 in

build)
    #
    cd $DIR
    echo "Enter to working folder '$DIR'"
    #
    # check Env | $varCC $varCCvalue
    crossc=`env | grep $varCC`
    strstr $crossc "$varCC=$varCCvalue"
    RT=$?
    if [ "$RT" -ne 0 ] ; then
        #echo "Valid variable $varCC in Environment Variable NOT PRESENT ($RT)"
        # try to set Valid variable $1 in Environment Variable
        export $set_envCC
        export $set_envPATH
        crossc=`env | grep $varCC`
        strstr $crossc "$varCC=$varCCvalue"
        RT=$?
        if [ "$RT" -ne 0 ] ; then
            echo "Valid variable $varCC in Environment Variable NOT PRESENT ($RT)"
            exit 1
        fi
    fi
    #
    delFile $SDKHDR_FILE
    delFile $SDKDEF_FILE
    delFile $SDKSRC_FILE
    #
    cmdHDR="$startDir/xmlParser $DIR/$inHdrFile hdr silent"
    cmdDEF="$startDir/xmlParser $DIR/$inDefFile def silent"
    cmdSRC="$startDir/xmlParser $DIR/$inSrcFile src silent"
    #
    # start xmlParser util for make sdk_hdr_folders_files
    $cmdHDR
    checkFile $SDKHDR_FILE
    # start xmlParser util for make sdk_macro_files
    $cmdDEF
    checkFile $SDKDEF_FILE
    # start xmlParser util for make sdk_src_names_files
    $cmdSRC
    checkFile $SDKSRC_FILE
    #
    LFILE=""
    for f in `ls *_FLASH.ld` ; do FILE=$(basename $f .ld) ;
        LFILE=$FILE.ld
    done
    if [ ${#LFILE} -lt 3 ] ; then
        exit 1
    fi
    #
    #              Define Paths and compile/link Flags
    #
    TMP=""
    echo "Get sdk hdr files (hdr_path from file $SDKHDR_FILE) :"
    while IFS= read -r line
    do
        if [ ${#line} -gt 0 ] ; then
            if [ "$line" != "$localINC" ]
            then
                TMP+="-I$sdkPATH$line "
            else
                TMP+="-I../$line "
            fi
        fi
    done  < "$SDKHDR_FILE"
    if [ ${#TMP} -lt 3 ] ; then
        TMP="-I../$localINC"
    fi
    INCSDK=$TMP
    echo "all headers path : $INCSDK"
    #
    TMP=""
    echo "Get sdk macro files (macro_value from file $SDKDEF_FILE) :"
    while IFS= read -r line
    do
        TMP+="-D$line "
    done  < "$SDKDEF_FILE"
    mCFLAG=$TMP
    echo "all macro : $mCFLAG"
    #
    cCFLAG="$aCFLAG $mCFLAG $cCFLAGop -MMD -MP"
    #
    #              Build folder present ?
    checkDir "$MKDIR"
    #              Enter to build folder
    cd $MKDIR
    echo "Enter to $DIR/$MKDIR"
    #              sturtup folder present ?
    checkDir "$ASMDIR"
    #              Src folder present ?
    checkDir "$SRC"
    #              remove 'object.list' file
    rm -f $OBJLIST
    #
    #                     Compile
    #
    echo "Building files from : ../$ASMDIR/"
    for f in `ls ../$ASMDIR/*.s` ; do FILE=$(basename $f .s) ;
        echo "Compile file '$FILE.s'"
        aCMD="${CROSS_COMPILE}as $aCFLAG -g -o $ASMDIR/$FILE.o ../$ASMDIR/$FILE.s"
        echo $aCMD
        $aCMD
        RT=$?
        if [ "$RT" -ne 0 ] ; then
            echo "Compile asm files ERROR ($RT)"
            exit 1
        else
            echo "\"$ASMDIR/$FILE.o\"" >> $OBJLIST
        fi
    done
    #
    echo "Building files from : ../$SRC/"
    for f in `ls ../$SRC/*.c` ; do FILE=$(basename $f .c) ;
        if [ ${#FILE} -gt 2 ] ; then
            echo "Compile file '$FILE.c' :"
            cCMD="${CROSS_COMPILE}gcc $INCSDK $cCFLAG -MF$SRC/$FILE.d -MT$SRC/$FILE.o -o $SRC/$FILE.o ../$SRC/$FILE.c"
            echo $cCMD
            $cCMD
            RT=$?
            if [ "$RT" -ne 0 ] ; then
                echo "Compile source files ERROR ($RT)"
                exit 1
            else
                echo "\"$SRC/$FILE.o\"" >> $OBJLIST
            fi
        else
            exit 1
        fi
    done
    #
    echo "Building resources files (list from file ../$SDKSRC_FILE) :"
    checkDir "$SDK"
    while IFS= read -r line
    do
        basename "$line"
        fn="$(basename -- $line)"
        echo "Compile file '$line.c' ($fn.c) to $SDK folder :"
        lCMD="${CROSS_COMPILE}gcc $INCSDK $cCFLAG -MF$SDK/$fn.d -MT$SDK/$fn.o -o $SDK/$fn.o $line.c"
        echo $lCMD
        $lCMD
        RT=$?
        if [ "$RT" -ne 0 ] ; then
            echo "Compile resources_files ERROR ($RT)"
            exit 1
        else
            echo "\"$SDK/$fn.o\"" >> $OBJLIST
        fi
    done  < "../$SDKSRC_FILE"

    #
    #                      Link
    #
    echo "Building target elf file"
    eCMD="${CROSS_COMPILE}gcc $aCFLAG $SPECS -T ../$LFILE -Wl,-Map=output.map -Wl,--gc-sections -o $PROJECT.elf @object.list -lm"
    echo $eCMD
    $eCMD
    RT=$?
    if [ "$RT" -ne 0 ] ; then
        echo "Link stage ERROR ($RT)"
        exit 1
    fi
    #
    #                  Create hex and bin files
    #
    echo "Generating hex,bin and Printing size information:"
    binCMD="${CROSS_COMPILE}objcopy -O binary $PROJECT.elf $PROJECT.bin"
    echo $binCMD
    $binCMD
    RT=$?
    if [ "$RT" -ne 0 ] ; then
        echo "Bin_make stage ERROR ($RT)"
        exit 1
    fi
    ls -la | grep "$PROJECT.*"
    #
    echo "Now you can write $PROJECT.bin to ROM. For exampl : stm32flash -w $MKDIR/$PROJECT.bin /dev/ttyUSB0"
    #
;;

remove)
    #
    cd $DIR
    echo "Enter to working folder '$DIR'"
    #
    if [ -d "$MKDIR" ]; then
        cd $MKDIR
        rm -f $PROJECT.elf $PROJECT.bin
        echo "Remove  $PROJECT.elf $PROJECT.bin files"
    fi
;;

delete)
    #
    cd $DIR
    echo "Enter to working folder '$DIR'"
    rm -f $SDKSRC_FILE
    rm -f $SDKDEF_FILE
    rm -f $SDKHDR_FILE
    #
    if [ -d "$MKDIR" ]; then
        rm -r -f $MKDIR
        echo "Remove folder '$MKDIR'"
    fi
;;


*)
    echo "No action present. Use : ./compile.sh build|remove|delete"
;;

esac



