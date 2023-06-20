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

// GetUsersHandle 获取所有用户信息处理方法
func GetUsersHandle(c *gin.Context) {
	hd := &GetUsersHandler{}
	handler.Handle(c, hd)
}

// GetUsersHandler 获取所有用户信息处理对象
type GetUsersHandler struct {
	Req  GetUsersRequest
	Resp GetUsersResponse
}

// GetUsersRequest 获取所有用户信息请求
type GetUsersRequest struct {
	Username string `json:"username"`
}

// GetUsersResponse 获取所有用户信息返回
type GetUsersResponse struct {
	common.BaseResponse
	Data []*UserData `json:"data"`
}

// BindReq 绑定参数
func (h *GetUsersHandler) BindReq(c *gin.Context) error {
	if err := c.ShouldBindBodyWith(&h.Req, binding.JSON); err != nil {
		msg := fmt.Sprintf("invalid request, bind error: %v", err)
		seelog.Error(msg)
		h.SetError(common.ErrorPanic, msg)
		return err
	}
	return nil
}

// AfterBindReq 绑定参数后校验参数
func (h *GetUsersHandler) AfterBindReq() error {
	return nil
}

// GetResponse 获取返回信息
func (h *GetUsersHandler) GetResponse() interface{} {
	return h.Resp
}

// SetError 设置错误信息
func (h *GetUsersHandler) SetError(code int, message string) {
	h.Resp.Code = code
	h.Resp.Message = message
}

// SetUsername 设置请求者用户名
func (h *GetUsersHandler) SetUsername(username string) {
	h.Req.Username = username
}

// NeedVerifyToken 是否需要验证token
func (h *GetUsersHandler) NeedVerifyToken() bool {
	return true
}

// Process 接口处理
func (h *GetUsersHandler) Process() {
	// 调用合约查询所有用户信息
	resp, err := fabric.ClientInstance.Query("", "getUsers", fabric.PackArgs([]string{}), nil, nil)
	if err != nil {
		msg := fmt.Sprintf("query chaincode error, %v", err)
		h.SetError(common.ErrorQuery, msg)
		seelog.Error(msg)
		return
	}
	users := []*chaincode.User{}
	if err := json.Unmarshal(resp.Responses[0].Response.Payload, &users); err != nil {
		msg := fmt.Sprintf("unmarshal User error, %v", err)
		h.SetError(common.ErrorUnmarshal, msg)
		seelog.Error(msg)
		return
	}
	h.Resp.Data = make([]*UserData, 0)
	for _, user := range users {
		h.Resp.Data = append(h.Resp.Data, &UserData{
			user.Username, user.Name, user.Organization, user.Role,
		})
	}
}
