package main

// 表明运行状态的常量定义
const(
	CHARGING = iota // 正在充电
	STOPPED 	    // 已经停止
	RUNNING         // 运行中
)

// 当前运行状态
var(
	status int
)

func getStatus() int {
	return status
}

func setStatus(s int){
	status = s
}
