#! /bin/sh
pwd
execname="clear.sh"

# 执行所以子目录下的指定脚本
function shellexec() {
    for dir_or_file in `ls .`
    do
        if [ -d $dir_or_file ]
        then
            # echo $dir_or_file
            cd $dir_or_file
                if [ -f $1 ]
                then
                    ./$1
                fi
            cd ..
        fi
    done
}

shellexec $execname
