# Golang 使用记录

## 使用包
- 设置环境变量GOPATH，在本目录中查找包：export GOPATH=`pwd`

## 下载包
- 由于golang.org上面的包被墙，使用github.com/golang代替，后再更名为golang.org/x

## 交叉编译
- 运行build.sh

## cgo
- 内嵌c代码，如hello.go中设置cflag(编译时包含文件)和ldflag(链接时包含的库文件)
- \#cgo CFLAGS: -I./uart
- \#cgo LDFLAGS: -linkmode external -extldflags "-lstdc++ -lgcc_eh -L/home/lhw/tina/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-musl/toolchain/lib -L./uart -luart -static"
- LDFLAGS: -l 的顺序对 link 有影响，自己编译的库在前，-lstdc++ 在后