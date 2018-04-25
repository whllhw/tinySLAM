package main
// #cgo CFLAGS: -I./uart
// #cgo LDFLAGS: -lctest -L./uart  -lstdc++
// #include "bridge.h"
import "C"
import "fmt"

func main(){
	C.pull_dy()
	C.pull_scan()
	C.Test()
	C.shutdown()
	fmt.Print("done")
}
// #//cgo LDFLAGS: -linkmode external -extldflags "-lstdc++ -lgcc_eh -L/home/lhw/tina/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-musl/toolchain/lib -L./uart -luart -static"

/*
 * LDFLAGS: -l 的顺序对 link 有影响，自己编译的库在前，-lstdc++ 在后
 */
