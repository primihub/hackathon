package fabric

import (
	"github.com/hyperledger/fabric-sdk-go/pkg/common/logging"
	"github.com/hyperledger/fabric-sdk-go/pkg/core/logging/api"
	"github.com/privacy-protection/demo-api/seelog"
)

// InitSDKLogger 初始化sdk日志
func InitSDKLogger() {
	logging.Initialize(&loggerProvider{})
}

type loggerProvider struct {
}

func (p *loggerProvider) GetLogger(module string) api.Logger {
	return p
}

func (p *loggerProvider) Fatal(v ...interface{}) {
	seelog.Error(v...)
}

func (p *loggerProvider) Fatalf(format string, v ...interface{}) {
	seelog.Errorf(format, v...)
}

func (p *loggerProvider) Fatalln(v ...interface{}) {
	seelog.Error(v...)
}

func (p *loggerProvider) Panic(v ...interface{}) {
	seelog.Error(v...)
}

func (p *loggerProvider) Panicf(format string, v ...interface{}) {
	seelog.Errorf(format, v...)
}

func (p *loggerProvider) Panicln(v ...interface{}) {
	seelog.Error(v...)
}

func (p *loggerProvider) Print(v ...interface{}) {
	seelog.Debug(v...)
}

func (p *loggerProvider) Printf(format string, v ...interface{}) {
	seelog.Debugf(format, v...)
}

func (p *loggerProvider) Println(v ...interface{}) {
	seelog.Debug(v...)
}

func (p *loggerProvider) Debug(args ...interface{}) {
	seelog.Debug(args...)
}

func (p *loggerProvider) Debugf(format string, args ...interface{}) {
	seelog.Debugf(format, args...)
}

func (p *loggerProvider) Debugln(args ...interface{}) {
	seelog.Debug(args...)
}

func (p *loggerProvider) Info(args ...interface{}) {
	seelog.Info(args...)
}

func (p *loggerProvider) Infof(format string, args ...interface{}) {
	seelog.Infof(format, args...)
}

func (p *loggerProvider) Infoln(args ...interface{}) {
	seelog.Info(args...)
}

func (p *loggerProvider) Warn(args ...interface{}) {
	seelog.Warn(args...)
}

func (p *loggerProvider) Warnf(format string, args ...interface{}) {
	seelog.Warnf(format, args...)
}

func (p *loggerProvider) Warnln(args ...interface{}) {
	seelog.Warn(args...)
}

func (p *loggerProvider) Error(args ...interface{}) {
	seelog.Error(args...)
}

func (p *loggerProvider) Errorf(format string, args ...interface{}) {
	seelog.Errorf(format, args...)
}

func (p *loggerProvider) Errorln(args ...interface{}) {
	seelog.Error(args...)
}
