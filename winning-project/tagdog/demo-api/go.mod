module github.com/privacy-protection/demo-api

require (
	github.com/cihub/seelog v0.0.0-20170130134532-f561c5e57575
	github.com/dgrijalva/jwt-go v3.2.0+incompatible
	github.com/gin-gonic/gin v1.7.4
	github.com/golang/protobuf v1.5.2
	github.com/google/uuid v1.3.0
	github.com/hyperledger/fabric-sdk-go v1.0.0
	github.com/petermattis/goid v0.0.0-20180202154549-b0b1615b78e5
	github.com/privacy-protection/kp-abe v0.0.0
	github.com/spf13/viper v1.7.1
	github.com/stretchr/testify v1.7.0
)

replace (
	github.com/hyperledger/fabric-sdk-go => ../fabric-sdk-go
	github.com/privacy-protection/kp-abe => ../kp-abe
)

go 1.13
