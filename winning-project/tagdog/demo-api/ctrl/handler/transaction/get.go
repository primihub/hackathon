package transaction

import (
	"encoding/json"
	"fmt"

	"github.com/gin-gonic/gin"
	"github.com/gin-gonic/gin/binding"
	"github.com/privacy-protection/demo-api/chaincode"
	"github.com/privacy-protection/demo-api/chaincode/util"
	"github.com/privacy-protection/demo-api/common"
	"github.com/privacy-protection/demo-api/crypto"
	"github.com/privacy-protection/demo-api/ctrl/handler"
	"github.com/privacy-protection/demo-api/fabric"
	"github.com/privacy-protection/demo-api/seelog"
)

// GetHandle 获取交易处理方法
func GetHandle(c *gin.Context) {
	hd := &GetHandler{}
	handler.Handle(c, hd)
}

// GetHandler 获取交易处理对象
type GetHandler struct {
	Req  GetRequest
	Resp GetResponse
}

// GetRequest 获取交易包请求
type GetRequest struct {
	Username string `json:"username"`
	Current  int    `json:"current" binding:"required"`
	PageSize int    `json:"pageSize" binding:"required"`
	Filter   bool   `json:"filter"`
}

// GetResponse 获取交易返回
type GetResponse struct {
	common.BaseResponse
	Total    int           `json:"total"`
	PageSize int           `json:"pageSize"`
	Current  int           `json:"current"`
	Data     []Transaction `json:"data"`
}

// Transaction 交易信息
type Transaction struct {
	ID          string      `json:"id"`
	ProductID   string      `json:"productId"`
	ProductName interface{} `json:"productName"`
	Transferor  interface{} `json:"transferor"`
	Transferee  interface{} `json:"transferee"`
	Turnover    int         `json:"turnover"`
	CreateTime  interface{} `json:"createTime"`
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
	// 获取所有交易
	resp, err := fabric.ClientInstance.Query("", "getTransactions", fabric.PackArgs([]string{}), nil, nil)
	if err != nil {
		msg := fmt.Sprintf("chaincode query error, %v", err)
		h.SetError(common.ErrorQuery, msg)
		seelog.Error(msg)
		return
	}
	transactions := []*chaincode.Transaction{}
	if err := json.Unmarshal(resp.Responses[0].Response.Payload, &transactions); err != nil {
		msg := fmt.Sprintf("unmarshal Transaction error, %v", err)
		h.SetError(common.ErrorQuery, msg)
		seelog.Error(msg)
		return
	}
	// 获取cpabe密钥
	cpKey, err := util.GetCPKey(h.Req.Username)
	if err != nil {
		msg := fmt.Sprintf("get cp key error, %v", err)
		h.SetError(common.ErrorInner, msg)
		seelog.Error(msg)
		return
	}
	// 获取kpabe密钥
	kpKeys, err := util.GetKPKeys(h.Req.Username)
	if err != nil {
		msg := fmt.Sprintf("get kp key error, %v", err)
		h.SetError(common.ErrorInner, msg)
		seelog.Error(msg)
		return
	}
	// 解密
	for _, transaction := range transactions {
		if err := crypto.ABEDecrypt(transaction, kpKeys, cpKey); err != nil {
			seelog.Errorf("aes decrypt error, %v", err)
		}
	}
	// 过滤
	if h.Req.Filter {
		filterTransactions := []*chaincode.Transaction{}
		for _, transaction := range transactions {
			if transaction.Transferee.OriginValue == h.Req.Username || transaction.Transferor.OriginValue == h.Req.Username {
				filterTransactions = append(filterTransactions, transaction)
			}
		}
		transactions = filterTransactions
	}
	h.Resp.Total = len(transactions)
	h.Resp.PageSize = h.Req.PageSize
	h.Resp.Current = h.Req.Current
	end := h.Resp.Current * h.Req.PageSize
	if h.Resp.Total < end {
		end = h.Resp.Total
	}
	h.Resp.Data = make([]Transaction, 0)
	for i := (h.Resp.Current - 1) * h.Req.PageSize; i < end; i++ {
		t := Transaction{
			ID:         transactions[i].ID,
			ProductID:  transactions[i].ProductID,
			Transferor: transactions[i].Transferor.OriginValue,
			Transferee: transactions[i].Transferee.OriginValue,
			Turnover:   transactions[i].Turnover,
			CreateTime: transactions[i].CreateTime.OriginValue,
		}

		// 获取对应的产品信息
		resp, err := fabric.ClientInstance.Query("", "getProduct", fabric.PackArgs([]string{transactions[i].ProductID}), nil, nil)
		if err != nil {
			msg := fmt.Sprintf("chaincode query error, %v", err)
			h.SetError(common.ErrorQuery, msg)
			seelog.Error(msg)
			return
		}
		product := &chaincode.Product{}
		if err := json.Unmarshal(resp.Responses[0].Response.Payload, product); err != nil {
			msg := fmt.Sprintf("unmarshal Product error, %v", err)
			h.SetError(common.ErrorQuery, msg)
			seelog.Error(msg)
			return
		}
		if err := crypto.ABEDecrypt(product, kpKeys, cpKey); err != nil {
			seelog.Errorf("aes decrypt error, %v", err)
		}

		t.ProductName = product.Name.OriginValue
		h.Resp.Data = append(h.Resp.Data, t)
	}
}
