package common

import (
	"strings"

	"github.com/gin-gonic/gin"
	"github.com/privacy-protection/demo-api/seelog"
)

// RouterRegister 路由注册对象
var RouterRegister RouterHandlerRegister

// RouterHandlerRegister 路由注册对象
type RouterHandlerRegister struct {
	Router         *gin.Engine
	RouterHandlers []RouterHandler
}

// RouterHandler 路由处理对象
type RouterHandler struct {
	Path    string          `json:"path"`
	Method  string          `json:"method"`
	Handler gin.HandlerFunc `json:"handler"`
}

// SetRouter 路由设置
func (r *RouterHandlerRegister) SetRouter(router *gin.Engine) {
	r.Router = router
}

// InitRouter 初始化
func (r *RouterHandlerRegister) InitRouter() {
	seelog.Infof("init router")
	for _, routerHandler := range r.RouterHandlers {
		switch strings.ToUpper(routerHandler.Method) {
		case "GET":
			r.Router.GET(routerHandler.Path, routerHandler.Handler)
			seelog.Infof("register GET router: %s", routerHandler.Path)
		case "POST":
			r.Router.POST(routerHandler.Path, routerHandler.Handler)
			seelog.Infof("register POST router: %s", routerHandler.Path)
		default:
			seelog.Errorf("not support method %s for path %s", routerHandler.Method, routerHandler.Path)
		}
	}
}

// RegisterRouterHandler 注册处理对象
func (r *RouterHandlerRegister) RegisterRouterHandler(rh RouterHandler) {
	if r.RouterHandlers == nil {
		r.RouterHandlers = make([]RouterHandler, 0)
	}
	r.RouterHandlers = append(r.RouterHandlers, rh)
}
