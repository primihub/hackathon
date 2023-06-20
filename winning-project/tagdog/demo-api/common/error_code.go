package common

const (
	// ErrorPanic panic
	ErrorPanic = 1 + iota
	// ErrorInvalidToken token不合法
	ErrorInvalidToken
	// ErrorInvalidParams 参数不合法
	ErrorInvalidParams
	// ErrorMarshal 序列化错误
	ErrorMarshal
	// ErrorUnmarshal 反序列化错误
	ErrorUnmarshal
	// ErrorExecuteCommand 执行命令行错误
	ErrorExecuteCommand
	// ErrorInvoke 执行合约错误
	ErrorInvoke
	// ErrorQuery 查询合约错误
	ErrorQuery
	// ErrorPassword 密码错误
	ErrorPassword
	// ErrorInner 内部错误
	ErrorInner
)

// BaseResponse 基础返回
type BaseResponse struct {
	Code    int    `json:"code"`
	Message string `json:"message"`
}
