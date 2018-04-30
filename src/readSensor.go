package main
// #cgo CFLAGS: -I./uart
// #cgo LDFLAGS: -lctest -L./uart  -lstdc++
// #include "bridge.h"
import "C"
import "fmt"

func pullDy() (<- chan C.struct_Dynamixel){
	ch := make(chan C.struct_Dynamixel)
	go func(){
		for ;; {
			ch <- C.pull_dy()
		}
	}()
	return ch
}

func pullScan() (<- chan C.struct_SCAN){
	ch := make(chan C.struct_SCAN)
	go func(){
		for ;; {
			ch <- C.pull_scan()
		}
	}()
	return ch
}

func main(){
	fmt.Print("start!")
	var ch <- chan C.struct_Dynamixel
	ch = pullDy()
	fmt.Println("ready to recv!")
	// 一个带有range子句的for语句会依次读取发往管道的值，直到该管道关闭
	for s:= range ch{
		fmt.Println(C.float(s.angle))
		fmt.Println(C.ushort(s.lspeed))
		fmt.Println(C.ushort(s.rspeed))
		fmt.Println(C.long(s.t))
		fmt.Println("---------------------------")
	}
	
	fmt.Print("done")
}

// #//cgo LDFLAGS: -linkmode external -extldflags "-lstdc++ -lgcc_eh -L/home/lhw/tina/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-musl/toolchain/lib -L./uart -luart -static"

/*
 * LDFLAGS: -l 的顺序对 link 有影响，自己编译的库在前，-lstdc++ 在后
 */
