package main

import (
	"fmt"

	"github.com/privacy-protection/demo-api/common"
	"github.com/privacy-protection/demo-api/conf"
	_ "github.com/privacy-protection/demo-api/ctrl"
	"github.com/privacy-protection/demo-api/fabric"
	"github.com/privacy-protection/demo-api/gin"
	"github.com/privacy-protection/demo-api/seelog"
)

func main() {
	defer seelog.Flush()
	seelog.Infof("start demo-api")

	if err := conf.InitConfig(); err != nil {
		seelog.Errorf("init config error, %v", err)
		return
	}

	fabric.InitSDKLogger()
	if err := fabric.InitClient(); err != nil {
		seelog.Errorf("init fabric sdk client error, %v", err)
		return
	}

	router := gin.CreateGin()
	common.RouterRegister.SetRouter(router)
	common.RouterRegister.InitRouter()
	if err := router.Run(fmt.Sprintf(":%d", conf.Conf.ServerConf.Port)); err != nil {
		seelog.Errorf("router run error, %v", err)
		return
	}

	seelog.Infof("stop demo-api")
}
