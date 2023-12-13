#! /bin/sh
# pwd

# prjName 指定新项目名
if [ -z $2 ]
then
    prjName="prjName"
else
    prjName=$2
fi

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

# 创建项目 分支 和 tag
git checkout -b $prjName
git tag $prjName/v1.0.0

# 重命名项目 分支 和 tag
#git branch -m $prjName newName
#git tag newName/v1.0.0 $prjName/v1.0.0
#git tag -d $prjName/v1.0.0


# clone Platform 库并切换到 dev 分支
git clone $remoteuser@$remotepath/Platform.git Platform
cd ./Platform/
git checkout dev


read -p "Press any key to continue" -t 15