# Golang 使用记录

## 使用包
- 设置环境变量GOPATH，在本目录中查找包：export GOPATH=`pwd`

## 下载包
- 由于golang.org上面的包被墙，使用github.com/golang代替，后再更名为golang.org/x

## 交叉编译
- 运行build.sh

## cgo
- 内嵌c代码，如hello.go中设置cflag和ldflag

