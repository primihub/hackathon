package policy

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

// GetHandle 获取cpabe策略处理方法
func GetHandle(c *gin.Context) {
	hd := &GetHandler{}
	handler.Handle(c, hd)
}

// GetHandler 获取cpabe策略处理对象
type GetHandler struct {
	Req  GetRequest
	Resp GetResponse
}

// GetRequest 获取cpabe策略请求
type GetRequest struct {
	Username string `json:"username"`
	Current  int    `json:"current" binding:"required"`
	PageSize int    `json:"pageSize" binding:"required"`
	Filter   bool   `json:"filter"`
}

// GetResponse 获获取cpabe策略返回
type GetResponse struct {
	common.BaseResponse
	Total    int                 `json:"total"`
	PageSize int                 `json:"pageSize"`
	Current  int                 `json:"current"`
	Data     []*chaincode.Policy `json:"data"`
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
	// 获取所有策略
	resp, err := fabric.ClientInstance.Query("", "getPolicies", fabric.PackArgs([]string{}), nil, nil)
	if err != nil {
		msg := fmt.Sprintf("chaincode query error, %v", err)
		h.SetError(common.ErrorQuery, msg)
		seelog.Error(msg)
		return
	}
	policies := []*chaincode.Policy{}
	if err := json.Unmarshal(resp.Responses[0].Response.Payload, &policies); err != nil {
		msg := fmt.Sprintf("unmarshal Policy error, %v", err)
		h.SetError(common.ErrorUnmarshal, msg)
		seelog.Error(msg)
		return
	}
	// 过滤
	if h.Req.Filter {
		filterPolicies := []*chaincode.Policy{}
		for _, policy := range policies {
			if policy.Creator == h.Req.Username {
				filterPolicies = append(filterPolicies, policy)
			}
		}
		policies = filterPolicies
	}
	h.Resp.Total = len(policies)
	h.Resp.PageSize = h.Req.PageSize
	h.Resp.Current = h.Req.Current
	end := h.Resp.Current * h.Req.PageSize
	if h.Resp.Total < end {
		end = h.Resp.Total
	}
	h.Resp.Data = make([]*chaincode.Policy, 0)
	for i := (h.Resp.Current - 1) * h.Req.PageSize; i < end; i++ {
		h.Resp.Data = append(h.Resp.Data, policies[i])
	}
}
