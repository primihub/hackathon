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

// ConfirmHandle 确认产品处理方法
func ConfirmHandle(c *gin.Context) {
	hd := &ConfirmHandler{}
	handler.Handle(c, hd)
}

// ConfirmHandler 确认产品处理对象
type ConfirmHandler struct {
	Req  ConfirmRequest
	Resp ConfirmResponse
}

// ConfirmRequest 确认产品请求
type ConfirmRequest struct {
	Username string `json:"username"`
	ID       string `json:"id" binding:"required"`
}

// ConfirmResponse 确认产品返回
type ConfirmResponse struct {
	common.BaseResponse
}

// BindReq 绑定参数
func (h *ConfirmHandler) BindReq(c *gin.Context) error {
	if err := c.ShouldBindBodyWith(&h.Req, binding.JSON); err != nil {
		msg := fmt.Sprintf("invalid request, bind error: %v", err)
		seelog.Error(msg)
		h.SetError(common.ErrorPanic, msg)
		return err
	}
	return nil
}

// AfterBindReq 绑定参数后校验参数
func (h *ConfirmHandler) AfterBindReq() error {
	return nil
}

// GetResponse 获取返回信息
func (h *ConfirmHandler) GetResponse() interface{} {
	return h.Resp
}

// SetError 设置错误信息
func (h *ConfirmHandler) SetError(code int, message string) {
	h.Resp.Code = code
	h.Resp.Message = message
}

// SetUsername 设置请求者用户名
func (h *ConfirmHandler) SetUsername(username string) {
	h.Req.Username = username
}

// NeedVerifyToken 是否需要验证token
func (h *ConfirmHandler) NeedVerifyToken() bool {
	return true
}

// Process 接口处理
func (h *ConfirmHandler) Process() {
	// TODO: 判断是否是自己的产品

	// 调用合约删除
	resp, err := fabric.ClientInstance.Invoke("", "confirmProduct", fabric.PackArgs([]string{h.Req.ID}), nil, nil)
	if err != nil {
		msg := fmt.Sprintf("invoke chaincode error, %v", err)
		h.SetError(common.ErrorInvoke, msg)
		seelog.Error(msg)
		return
	}
	seelog.Infof("tx id: %s, valid code: %d", resp.TransactionID, resp.TxValidationCode)
}
