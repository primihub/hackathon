package asset

import (
	"fmt"

	"github.com/gin-gonic/gin"
	"github.com/gin-gonic/gin/binding"
	"github.com/privacy-protection/demo-api/common"
	"github.com/privacy-protection/demo-api/ctrl/handler"
	"github.com/privacy-protection/demo-api/fabric"
	"github.com/privacy-protection/demo-api/seelog"
)

// DeleteHandle 删除资产处理方法
func DeleteHandle(c *gin.Context) {
	hd := &DeleteHandler{}
	handler.Handle(c, hd)
}

// DeleteHandler 删除资产处理对象
type DeleteHandler struct {
	Req  DeleteRequest
	Resp DeleteResponse
}

// DeleteRequest 删除资产请求
type DeleteRequest struct {
	Username string `json:"username"`
	ID       string `json:"id" binding:"required"`
}

// DeleteResponse 删除资产返回
type DeleteResponse struct {
	common.BaseResponse
}

// BindReq 绑定参数
func (h *DeleteHandler) BindReq(c *gin.Context) error {
	if err := c.ShouldBindBodyWith(&h.Req, binding.JSON); err != nil {
		msg := fmt.Sprintf("invalid request, bind error: %v", err)
		seelog.Error(msg)
		h.SetError(common.ErrorPanic, msg)
		return err
	}
	return nil
}

// AfterBindReq 绑定参数后校验参数
func (h *DeleteHandler) AfterBindReq() error {
	return nil
}

// GetResponse 获取返回信息
func (h *DeleteHandler) GetResponse() interface{} {
	return h.Resp
}

// SetError 设置错误信息
func (h *DeleteHandler) SetError(code int, message string) {
	h.Resp.Code = code
	h.Resp.Message = message
}

// SetUsername 设置请求者用户名
func (h *DeleteHandler) SetUsername(username string) {
	h.Req.Username = username
}

// NeedVerifyToken 是否需要验证token
func (h *DeleteHandler) NeedVerifyToken() bool {
	return true
}

// Process 接口处理
func (h *DeleteHandler) Process() {
	// TODO: 判断是否是自己的资产

	// 调用合约删除
	resp, err := fabric.ClientInstance.Invoke("", "deleteAsset", fabric.PackArgs([]string{h.Req.ID}), nil, nil)
	if err != nil {
		msg := fmt.Sprintf("invoke chaincode error, %v", err)
		h.SetError(common.ErrorInvoke, msg)
		seelog.Error(msg)
		return
	}
	seelog.Infof("tx id: %s, valid code: %d", resp.TransactionID, resp.TxValidationCode)
}
