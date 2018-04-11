package main
// #//cgo LDFLAGS: -linkmode external -extldflags "-lgcc_eh -L/home/lhw/tina/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-musl/toolchain/lib -static"
// #include <stdio.h>
// #include <stdlib.h>
/*
void print(char *str) {
    printf("%s\n", str);
}
*/
import "C"
 
import "unsafe"
 
func main() {
    s := "Hello Cgo"
    cs := C.CString(s)
    C.print(cs)
    C.free(unsafe.Pointer(cs))
}
// #//cgo LDFLAGS: -linkmode external -extldflags "-static"
