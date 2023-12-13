#! /bin/sh
# pwd

# localdir 指定本地工作目录
if [ -z $1 ]
then
    localdir="./work_new"
else
    localdir=$1
fi
# remotepath 指定远程库路径
remotepath="192.168.6.10:/mnt/raid/repo_new"
# remoteuser 指定运程用户名
remoteuser=git

# clone Projects 库并切换到 dev 分支
git clone $remoteuser@$remotepath/Projects.git $localdir
cd $localdir
git checkout dev
git checkout tags/v1.0.0 -b branch_v1.0.0

# clone Platform 库并切换到 dev 分支
git clone $remoteuser@$remotepath/Platform.git Platform
cd ./Platform/
git checkout dev

read -p "Press any key to continue" -t 15