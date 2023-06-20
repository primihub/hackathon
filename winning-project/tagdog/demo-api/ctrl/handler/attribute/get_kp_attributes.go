package attribute

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

// GetKPAttributesHandle 获取kpabe属性处理方法
func GetKPAttributesHandle(c *gin.Context) {
	hd := &GetKPAttributesHandler{}
	handler.Handle(c, hd)
}

// GetKPAttributesHandler 获取kpabe属性处理对象
type GetKPAttributesHandler struct {
	Req  GetKPAttributesRequest
	Resp GetKPAttributesResponse
}

// GetKPAttributesRequest 获取kpabe策略请求
type GetKPAttributesRequest struct {
	Username string `json:"username"`
}

// GetKPAttributesResponse 获获取kpabe属性返回
type GetKPAttributesResponse struct {
	common.BaseResponse
	Total int                      `json:"total"`
	Data  []*chaincode.KPAttribute `json:"data"`
}

// BindReq 绑定参数
func (h *GetKPAttributesHandler) BindReq(c *gin.Context) error {
	if err := c.ShouldBindBodyWith(&h.Req, binding.JSON); err != nil {
		msg := fmt.Sprintf("invalid request, bind error: %v", err)
		seelog.Error(msg)
		h.SetError(common.ErrorPanic, msg)
		return err
	}
	return nil
}

// AfterBindReq 绑定参数后校验参数
func (h *GetKPAttributesHandler) AfterBindReq() error {
	return nil
}

// GetResponse 获取返回信息
func (h *GetKPAttributesHandler) GetResponse() interface{} {
	return h.Resp
}

// SetError 设置错误信息
func (h *GetKPAttributesHandler) SetError(code int, message string) {
	h.Resp.Code = code
	h.Resp.Message = message
}

// SetUsername 设置请求者用户名
func (h *GetKPAttributesHandler) SetUsername(username string) {
	h.Req.Username = username
}

// NeedVerifyToken 是否需要验证token
func (h *GetKPAttributesHandler) NeedVerifyToken() bool {
	return true
}

// Process 接口处理
func (h *GetKPAttributesHandler) Process() {
	// 获取所有策略
	resp, err := fabric.ClientInstance.Query("", "getKPAttributes", fabric.PackArgs([]string{}), nil, nil)
	if err != nil {
		msg := fmt.Sprintf("chaincode query error, %v", err)
		h.SetError(common.ErrorQuery, msg)
		seelog.Error(msg)
		return
	}
	result := []*chaincode.KPAttribute{}
	if err := json.Unmarshal(resp.Responses[0].Response.Payload, &result); err != nil {
		msg := fmt.Sprintf("unmarshal KPAttribute error, %v", err)
		h.SetError(common.ErrorUnmarshal, msg)
		seelog.Error(msg)
		return
	}
	h.Resp.Total = len(result)
	for i := 0; i < h.Resp.Total; i++ {
		h.Resp.Data = append(h.Resp.Data, result[i])
	}
}
