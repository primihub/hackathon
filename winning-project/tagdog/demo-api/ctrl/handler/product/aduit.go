package product

import (
	"fmt"

	"github.com/gin-gonic/gin"
	"github.com/gin-gonic/gin/binding"
	"github.com/privacy-protection/demo-api/common"
	"github.com/privacy-protection/demo-api/ctrl/handler"
	"github.com/privacy-protection/demo-api/fabric"
	"github.com/privacy-protection/demo-api/seelog"
)

// AuditHandle 审计产品处理方法
func AuditHandle(c *gin.Context) {
	hd := &AuditHandler{}
	handler.Handle(c, hd)
}

// AuditHandler 审计产品处理对象
type AuditHandler struct {
	Req  AuditRequest
	Resp AuditResponse
}

// AuditRequest 审计产品请求
type AuditRequest struct {
	Username    string `json:"username"`
	ID          string `json:"id" binding:"required"`
	AduitResult bool   `json:"aduitResult"`
}

// AuditResponse 审计产品返回
type AuditResponse struct {
	common.BaseResponse
}

// BindReq 绑定参数
func (h *AuditHandler) BindReq(c *gin.Context) error {
	if err := c.ShouldBindBodyWith(&h.Req, binding.JSON); err != nil {
		msg := fmt.Sprintf("invalid request, bind error: %v", err)
		seelog.Error(msg)
		h.SetError(common.ErrorPanic, msg)
		return err
	}
	return nil
}

// AfterBindReq 绑定参数后校验参数
func (h *AuditHandler) AfterBindReq() error {
	return nil
}

// GetResponse 获取返回信息
func (h *AuditHandler) GetResponse() interface{} {
	return h.Resp
}

// SetError 设置错误信息
func (h *AuditHandler) SetError(code int, message string) {
	h.Resp.Code = code
	h.Resp.Message = message
}

// SetUsername 设置请求者用户名
func (h *AuditHandler) SetUsername(username string) {
	h.Req.Username = username
}

// NeedVerifyToken 是否需要验证token
func (h *AuditHandler) NeedVerifyToken() bool {
	return true
}

// Process 接口处理
func (h *AuditHandler) Process() {
	// TODO: 判断是否是自己的产品

	auditResult := "Y"
	if !h.Req.AduitResult {
		auditResult = "N"
	}
	// 调用合约删除
	resp, err := fabric.ClientInstance.Invoke("", "auditProduct", fabric.PackArgs([]string{h.Req.ID, auditResult}), nil, nil)
	if err != nil {
		msg := fmt.Sprintf("invoke chaincode error, %v", err)
		h.SetError(common.ErrorInvoke, msg)
		seelog.Error(msg)
		return
	}
	seelog.Infof("tx id: %s, valid code: %d", resp.TransactionID, resp.TxValidationCode)
}
