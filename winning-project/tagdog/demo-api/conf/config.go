package conf

import (
	"encoding/pem"
	"fmt"
	"io/ioutil"
	"strings"

	"github.com/golang/protobuf/proto"
	"github.com/privacy-protection/demo-api/seelog"
	"github.com/privacy-protection/kp-abe/core"
	"github.com/spf13/viper"
)

// InitConfig 初始化配置
func InitConfig() error {
	v := viper.New()
	v.SetEnvPrefix(DemoConfigPrefix)
	v.AutomaticEnv()
	replacer := strings.NewReplacer(".", "_")
	v.SetEnvKeyReplacer(replacer)
	v.SetConfigFile(DefaultDemoConfigFile)
	v.SetConfigType("yaml")
	if err := v.ReadInConfig(); err != nil {
		return fmt.Errorf("read config error, %v", err)
	}
	for _, key := range v.AllKeys() {
		seelog.Infof("%s=%v", key, v.Get(key))
	}
	Conf = &Config{}
	if err := v.Unmarshal(Conf); err != nil {
		return fmt.Errorf("unmarshal Config error, %v", err)
	}
	if err := initKPMasterKey(); err != nil {
		return fmt.Errorf("init kp master key error, %v", err)
	}
	return nil
}

func initKPMasterKey() error {
	b, err := ioutil.ReadFile(Conf.KPABEConf.MasterKeyPath)
	if err != nil {
		return fmt.Errorf("read file error, %v", err)
	}
	block, _ := pem.Decode(b)
	Conf.KPABEConf.MasterKey = &core.MasterKey{}
	if err := proto.Unmarshal(block.Bytes, Conf.KPABEConf.MasterKey); err != nil {
		return fmt.Errorf("unmarshal MasterKey error, %v", err)
	}
	return nil
}
