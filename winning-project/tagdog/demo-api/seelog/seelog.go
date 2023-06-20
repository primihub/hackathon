package seelog

import (
	"fmt"
	"log"
	"path/filepath"
	"runtime"

	"github.com/cihub/seelog"
	"github.com/petermattis/goid"
)

func init() {
	seelog.RegisterCustomFormatter("ServiceName", createAppNameFormatter)
	logger, err := seelog.LoggerFromConfigAsString(seelogConfig)
	if err != nil {
		log.Fatalf("parse seelog config file error, %v", err)
	}
	seelog.ReplaceLogger(logger)
}

const (
	// LogLevelError error级别
	LogLevelError = "ERROR"
	// LogLevelInfo info级别
	LogLevelInfo = "INFO"
	// LogLevelDebug debug级别
	LogLevelDebug = "DEBUG"
	// LogLevelWarn warn级别
	LogLevelWarn = "WARN"
)

func createAppNameFormatter(params string) seelog.FormatterFunc {
	return func(message string, level seelog.LogLevel, context seelog.LogContextInterface) interface{} {
		serviceName := "demo-api"
		return serviceName
	}
}

// Errorf error级别格式化日志
func Errorf(format string, params ...interface{}) error {
	prefix := getPrefix(LogLevelError)
	return seelog.Errorf(prefix+format, params...)
}

// Error error级别日志
func Error(params ...interface{}) error {
	prefix := getPrefix(LogLevelError)
	var newParams []interface{}
	newParams = append(newParams, prefix)
	for _, param := range params {
		newParams = append(newParams, param)
	}
	return seelog.Error(prefix, newParams)
}

// Infof info级别格式化日志
func Infof(format string, params ...interface{}) {
	seelog.Infof(getPrefix(LogLevelInfo)+format, params...)
}

// Info info级别日志
func Info(params ...interface{}) {
	prefix := getPrefix(LogLevelInfo)
	var newParams []interface{}
	newParams = append(newParams, prefix)
	for _, param := range params {
		newParams = append(newParams, param)
	}
	seelog.Info(newParams...)
}

// Debugf debug级别格式化日志
func Debugf(format string, params ...interface{}) {
	seelog.Infof(getPrefix(LogLevelDebug)+format, params...)
}

// Debug debug级别日志
func Debug(params ...interface{}) {
	prefix := getPrefix(LogLevelDebug)
	var newParams []interface{}
	newParams = append(newParams, prefix)
	for _, param := range params {
		newParams = append(newParams, param)
	}
	seelog.Debug(newParams...)
}

// Warnf warn级别格式化日志
func Warnf(format string, params ...interface{}) {
	seelog.Infof(getPrefix(LogLevelWarn)+format, params...)
}

// Warn warn级别日志
func Warn(params ...interface{}) {
	prefix := getPrefix(LogLevelWarn)
	var newParams []interface{}
	newParams = append(newParams, prefix)
	for _, param := range params {
		newParams = append(newParams, param)
	}
	seelog.Debug(newParams...)
}

// Flush 刷新日志到文件
func Flush() {
	seelog.Flush()
}

func getPrefix(level string) string {
	callerInfo := getCallerName()
	return fmt.Sprintf("%s [%d] %s: ", level, goid.Get(), callerInfo)
}

func getCallerName() string {
	_, file, line, _ := runtime.Caller(3)
	return fmt.Sprintf("%s.%d", filepath.Base(file), line)
}

var seelogConfig string = `
<seelog minlevel="trace">
	<outputs formatid="fmt_info">
		<filter levels="trace,debug,info,warn,error,critical">
			<rollingfile formatid="fmt_info" type="size" filename="./logs/application.log" maxsize="33554432" maxrolls="64"/>
		</filter>
		<console/>
	</outputs>
	<formats>
		<format id="fmt_info" format="%Date(2006-01-02 15:04:05.999) %ServiceName %Msg%n" />
		<format id="fmt_err" format="%Date(2006-01-02 15:04:05.999) %ServiceName %Msg%n" />
	</formats>
</seelog>
`
