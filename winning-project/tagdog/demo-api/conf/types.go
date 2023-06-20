package conf

import (
	"github.com/privacy-protection/kp-abe/core"
)

// Conf 配置对象
var Conf *Config

// Config 配置信息
type Config struct {
	ServerConf ServerConfig `mapstructure:"server"`
	CPABEConf  CPABEConfig  `mapstructure:"cpabe"`
	KPABEConf  KPABEConfig  `mapstructure:"kpabe"`
	CryptoConf CryptoConfig `mapstructure:"crypto"`
}

// ServerConfig 服务端配置
type ServerConfig struct {
	Port            int    `mapstructure:"port"`
	TokenExpireTime int    `mapstructure:"token_expire_time"`
	TokenSecret     string `mapstructure:"token_secret"`
}

// KPABEConfig kpae配置
type KPABEConfig struct {
	MasterKeyPath string `mapstructure:"master_key_path"`
	MasterKey     *core.MasterKey
}

// CPABEConfig cpabe配置
type CPABEConfig struct {
	Path string `mapstructure:"path"`
}

// CryptoConfig 密码配置
type CryptoConfig struct {
	AESKey string `mapstructure:"aes_key"`
}

const (
	// DemoConfigPrefix 配置前缀
	DemoConfigPrefix = "DEMO"
	// DefaultDemoConfigFile 默认配置文件路径
	DefaultDemoConfigFile = "config/demo_config.yaml"
	// DefaultSDKConfigFile 默认sdk配置文件路径
	DefaultSDKConfigFile = "config/sdk_config.yaml"
)
