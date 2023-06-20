package user

import (
	"encoding/json"
	"fmt"

	"github.com/gin-gonic/gin"
	"github.com/gin-gonic/gin/binding"
	"github.com/privacy-protection/demo-api/chaincode"
	"github.com/privacy-protection/demo-api/common"
	"github.com/privacy-protection/demo-api/ctrl/handler"
	"github.com/privacy-protection/demo-api/fabric"
	"github.com/privacy-protection/demo-api/seelog"
)

// GetUserHandle 获取用户信息处理方法
func GetUserHandle(c *gin.Context) {
	hd := &GetUserHandler{}
	handler.Handle(c, hd)
}

// GetUserHandler 获取用户信息处理对象
type GetUserHandler struct {
	Req  GetUserRequest
	Resp GetUserResponse
}

// GetUserRequest 获取用户信息请求
type GetUserRequest struct {
	Username string `json:"username"`
}

// GetUserResponse 获取用户信息返回
type GetUserResponse struct {
	common.BaseResponse
	User UserData `json:"user"`
}

// UserData 用户数据
type UserData struct {
	Username     string `json:"username"`
	Name         string `json:"name"`
	Organization string `json:"organization"`
	Role         string `json:"role"`
}

// BindReq 绑定参数
func (h *GetUserHandler) BindReq(c *gin.Context) error {
	if err := c.ShouldBindBodyWith(&h.Req, binding.JSON); err != nil {
		msg := fmt.Sprintf("invalid request, bind error: %v", err)
		seelog.Error(msg)
		h.SetError(common.ErrorPanic, msg)
		return err
	}
	return nil
}

// AfterBindReq 绑定参数后校验参数
func (h *GetUserHandler) AfterBindReq() error {
	return nil
}

// GetResponse 获取返回信息
func (h *GetUserHandler) GetResponse() interface{} {
	return h.Resp
}

// SetError 设置错误信息
func (h *GetUserHandler) SetError(code int, message string) {
	h.Resp.Code = code
	h.Resp.Message = message
}

// SetUsername 设置请求者用户名
func (h *GetUserHandler) SetUsername(username string) {
	h.Req.Username = username
}

// NeedVerifyToken 是否需要验证token
func (h *GetUserHandler) NeedVerifyToken() bool {
	return true
}

// Process 接口处理
func (h *GetUserHandler) Process() {
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

	h.Resp.User = UserData{user.Username, user.Name, user.Organization, user.Role}
}
