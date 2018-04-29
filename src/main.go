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
import (
	"github.com/labstack/echo"
	"net/http"
	"encoding/json"
)

type User struct {
	Name  string `json:"name"`
	Email string `json:"email"`
}

type response struct {
	message string
	error   int
}

func backCharge(c echo.Context) error {
	// 这里调用其他的函数，返回结果

	return c.JSON(http.StatusOK, new(response))
}
func hello(c echo.Context) error {

	u := new(User)
	u.Name = "will"
	u.Email = "will@will.com"
	return c.JSON(http.StatusOK, u)
}

func clean(c echo.Context) error {
	// 调用清洁程序
	return c.JSON(http.StatusOK, new(response))
}

func getMap(c echo.Context) error {
	u := new(response)
	c.Response().Header().Set(echo.HeaderContentType, echo.MIMEApplicationJSONCharsetUTF8)
	c.Response().WriteHeader(http.StatusOK)
	return json.NewEncoder(c.Response()).Encode(u)
}

func main() {
	s := "Hello Cgo\n"
	cs := C.CString(s)
	C.print(cs)
	C.free(unsafe.Pointer(cs))
	e := echo.New()
	e.GET("/", hello)
	e.GET("/backCharge", backCharge)
	e.GET("/clean", clean)
	e.GET("/map", getMap)
	e.Logger.Fatal(e.Start(":1323"))
}
