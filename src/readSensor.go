package main
// #cgo CFLAGS: -I./uart
// #cgo LDFLAGS: -lctest -L./uart  -lstdc++
// #include "bridge.h"
import "C"
import (
	"fmt"
	"unsafe"
	"os"
	"os/signal"
	"syscall"
	"time"
)

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

func clean(){
	sigs := make(chan os.Signal, 1)
	signal.Notify(sigs, syscall.SIGINT, syscall.SIGTERM)
    <-sigs
	C.shutdown()
    time.Sleep(2000)
    fmt.Println("cleaned and exiting...")
    os.Exit(0) // exit program
}

func main(){
	fmt.Print("start!")
	//var ch <- chan C.struct_Dynamixel
	//ch = pullDy()
	//fmt.Println("ready to recv!")
	//// 一个带有range子句的for语句会依次读取发往管道的值，直到该管道关闭
	//for s:= range ch {
	//	fmt.Println(C.float(s.angle))
	//	fmt.Println(C.ushort(s.lspeed))
	//	fmt.Println(C.ushort(s.rspeed))
	//	fmt.Println(C.long(s.t))
	//	fmt.Println("---------------------------")
	//}
	var ch <- chan C.struct_SCAN
	ch = pullScan()
	fmt.Println("ready to recv!")
	go clean() // 释放资源
	for s:= range ch{
		fmt.Println(CArrayToGoArray_float32(unsafe.Pointer(&s.ranges[0]),360))
		fmt.Println(C.float(s.time_increment))
		fmt.Println(C.long(s.t))
		fmt.Println("---------------------------")
	}
	fmt.Print("done")
}

func CArrayToGoArray_float32(cArray unsafe.Pointer, size int) (goArray []float32) {
    p := uintptr(cArray)
    for i :=0; i < size; i++ {
        j := *(*float32)(unsafe.Pointer(p))
        goArray = append(goArray, j)
        p += unsafe.Sizeof(j)
    }
    return
}
// #//cgo LDFLAGS: -linkmode external -extldflags "-lstdc++ -lgcc_eh -L/home/lhw/tina/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-musl/toolchain/lib -L./uart -luart -static"

/*
 * LDFLAGS: -l 的顺序对 link 有影响，自己编译的库在前，-lstdc++ 在后
 */
