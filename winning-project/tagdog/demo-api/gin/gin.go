package gin

import (
	"io/ioutil"

	"github.com/gin-gonic/gin"
)

// CreateGin 创建gin对象
func CreateGin() *gin.Engine {
	gin.SetMode(gin.ReleaseMode)
	gin.DefaultWriter = ioutil.Discard
	engine := gin.Default()
	engine.Use(panicRecover())
	engine.Use(infoLog())
	return engine
}
