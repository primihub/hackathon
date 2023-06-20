package user

import (
	"crypto/sha256"
	"encoding/base64"
	"encoding/json"
	"fmt"

	"github.com/gin-gonic/gin"
	"github.com/gin-gonic/gin/binding"
	"github.com/privacy-protection/demo-api/chaincode"
	"github.com/privacy-protection/demo-api/common"
	"github.com/privacy-protection/demo-api/ctrl/handler"
	"github.com/privacy-protection/demo-api/ctrl/token"
	"github.com/privacy-protection/demo-api/fabric"
	"github.com/privacy-protection/demo-api/seelog"
)

// LoginHandle 登录处理方法
func LoginHandle(c *gin.Context) {
	hd := &LoginHandler{}
	handler.Handle(c, hd)
}

// LoginHandler 登录处理对象
type LoginHandler struct {
	Req  LoginRequest
	Resp LoginResponse
}

// LoginRequest 登录请求
type LoginRequest struct {
	Username string `json:"username" binding:"required"`
	Password string `json:"password" binding:"required"`
}

// LoginResponse 登录返回
type LoginResponse struct {
	common.BaseResponse
	Token string `json:"token"`
}

// BindReq 绑定参数
func (h *LoginHandler) BindReq(c *gin.Context) error {
	if err := c.ShouldBindBodyWith(&h.Req, binding.JSON); err != nil {
		msg := fmt.Sprintf("invalid request, bind error: %v", err)
		seelog.Error(msg)
		h.SetError(common.ErrorPanic, msg)
		return err
	}
	return nil
}

// AfterBindReq 绑定参数后校验参数
func (h *LoginHandler) AfterBindReq() error {
	return nil
}

// GetResponse 获取返回信息
func (h *LoginHandler) GetResponse() interface{} {
	return h.Resp
}

// SetError 设置错误信息
func (h *LoginHandler) SetError(code int, message string) {
	h.Resp.Code = code
	h.Resp.Message = message
}

// SetUsername 设置请求者用户名
func (h *LoginHandler) SetUsername(username string) {
}

// NeedVerifyToken 是否需要验证token
func (h *LoginHandler) NeedVerifyToken() bool {
	return false
}

// Process 接口处理
func (h *LoginHandler) Process() {
	// 调用合约查询用户信息
	resp, err := fabric.ClientInstance.Query("", "getUser", fabric.PackArgs([]string{h.Req.Username}), nil, nil)
	if err != nil {
		msg := fmt.Sprintf("query chaincode error, %v", err)
		h.SetError(common.ErrorQuery, msg)
		seelog.Error(msg)
		return
	}
	user := &chaincode.User{}
	if err := json.Unmarshal(resp.Responses[0].Response.Payload, user); err != nil {
		msg := fmt.Sprintf("unmarshal User error, %v", err)
		h.SetError(common.ErrorUnmarshal, msg)
		seelog.Error(msg)
		return
	}

	// 密码判断
	hash := sha256.New()
	hash.Sum([]byte(h.Req.Password))
	hp := hash.Sum(nil)
	p := base64.StdEncoding.EncodeToString(hp)
	if p != user.Password {
		msg := fmt.Sprintf("incorrect password")
		h.SetError(common.ErrorPassword, msg)
		seelog.Error(msg)
		return
	}

	t, err := token.GenerateToken(h.Req.Username)
	if err != nil {
		msg := fmt.Sprintf("generate token error, %v", err)
		h.SetError(common.ErrorInner, msg)
		seelog.Error(msg)
		return
	}

	h.Resp.Token = t
}
