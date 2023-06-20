package policy

import (
	"encoding/json"
	"fmt"
	"time"

	"github.com/gin-gonic/gin"
	"github.com/gin-gonic/gin/binding"
	"github.com/privacy-protection/demo-api/chaincode"
	"github.com/privacy-protection/demo-api/common"
	"github.com/privacy-protection/demo-api/ctrl/handler"
	"github.com/privacy-protection/demo-api/fabric"
	"github.com/privacy-protection/demo-api/seelog"
)

// CreateHandle 创建cpabe策略处理方法
func CreateHandle(c *gin.Context) {
	hd := &CreateHandler{}
	handler.Handle(c, hd)
}

// CreateHandler 创建cpabe策略处理对象
type CreateHandler struct {
	Req  CreateRequest
	Resp CreateResponse
}

// CreateRequest 创建cpabe策略请求
type CreateRequest struct {
	Username    string `json:"username"`
	Description string `json:"description" binding:"required"`
	Value       string `json:"value" binding:"required"`
}

// CreateResponse 创建cpabe策略返回
type CreateResponse struct {
	common.BaseResponse
}

// BindReq 绑定参数
func (h *CreateHandler) BindReq(c *gin.Context) error {
	if err := c.ShouldBindBodyWith(&h.Req, binding.JSON); err != nil {
		msg := fmt.Sprintf("invalid request, bind error: %v", err)
		seelog.Error(msg)
		h.SetError(common.ErrorPanic, msg)
		return err
	}
	return nil
}

// AfterBindReq 绑定参数后校验参数
func (h *CreateHandler) AfterBindReq() error {
	return nil
}

// GetResponse 获取返回信息
func (h *CreateHandler) GetResponse() interface{} {
	return h.Resp
}

// SetError 设置错误信息
func (h *CreateHandler) SetError(code int, message string) {
	h.Resp.Code = code
	h.Resp.Message = message
}

// SetUsername 设置请求者用户名
func (h *CreateHandler) SetUsername(username string) {
	h.Req.Username = username
}

// NeedVerifyToken 是否需要验证token
func (h *CreateHandler) NeedVerifyToken() bool {
	return true
}

// Process 接口处理
func (h *CreateHandler) Process() {
	// 封装数据
	policy := &chaincode.Policy{
		Description: h.Req.Description,
		Creator:     h.Req.Username,
		Value:       h.Req.Value,
		CreateTime:  time.Now().Format(common.TimeFormat),
	}
	b, err := json.Marshal(policy)
	if err != nil {
		msg := fmt.Sprintf("marshal Policy error, %v", err)
		h.SetError(common.ErrorMarshal, msg)
		seelog.Error(msg)
		return
	}
	// 调用合约存证
	resp, err := fabric.ClientInstance.Invoke("", "createPolicy", fabric.PackArgs([]string{string(b)}), nil, nil)
	if err != nil {
		msg := fmt.Sprintf("invoke chaincode error, %v", err)
		h.SetError(common.ErrorInvoke, msg)
		seelog.Error(msg)
		return
	}
	seelog.Infof("tx id: %s, valid code: %d", resp.TransactionID, resp.TxValidationCode)
}
