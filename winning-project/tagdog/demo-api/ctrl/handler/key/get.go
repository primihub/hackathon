package key

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

// GetHandle 获取kpabe密钥处理方法
func GetHandle(c *gin.Context) {
	hd := &GetHandler{}
	handler.Handle(c, hd)
}

// GetHandler  获取kpabe密钥处理对象
type GetHandler struct {
	Req  GetRequest
	Resp GetResponse
}

// GetRequest  获取kpabe密钥请求
type GetRequest struct {
	Username string `json:"username"`
}

// GetResponse 获取kpabe密钥返回
type GetResponse struct {
	common.BaseResponse
	Total int                `json:"total"`
	Data  []*chaincode.KPKey `json:"data"`
}

// BindReq 绑定参数
func (h *GetHandler) BindReq(c *gin.Context) error {
	if err := c.ShouldBindBodyWith(&h.Req, binding.JSON); err != nil {
		msg := fmt.Sprintf("invalid request, bind error: %v", err)
		seelog.Error(msg)
		h.SetError(common.ErrorPanic, msg)
		return err
	}
	return nil
}

// AfterBindReq 绑定参数后校验参数
func (h *GetHandler) AfterBindReq() error {
	return nil
}

// GetResponse 获取返回信息
func (h *GetHandler) GetResponse() interface{} {
	return h.Resp
}

// SetError 设置错误信息
func (h *GetHandler) SetError(code int, message string) {
	h.Resp.Code = code
	h.Resp.Message = message
}

// SetUsername 设置请求者用户名
func (h *GetHandler) SetUsername(username string) {
	h.Req.Username = username
}

// NeedVerifyToken 是否需要验证token
func (h *GetHandler) NeedVerifyToken() bool {
	return true
}

// Process 接口处理
func (h *GetHandler) Process() {
	// 调用合约获取密钥
	resp, err := fabric.ClientInstance.Query("", "getKeys", fabric.PackArgs([]string{h.Req.Username}), nil, nil)
	if err != nil {
		msg := fmt.Sprintf("chaincode query error, %v", err)
		h.SetError(common.ErrorQuery, msg)
		seelog.Error(msg)
		return
	}
	key := []*chaincode.KPKey{}
	if err := json.Unmarshal(resp.Responses[0].Response.Payload, &key); err != nil {
		msg := fmt.Sprintf("unmarshal KPKey error, %v", err)
		h.SetError(common.ErrorUnmarshal, msg)
		seelog.Error(msg)
		return
	}
	h.Resp.Total = len(key)
	h.Resp.Data = key
}
