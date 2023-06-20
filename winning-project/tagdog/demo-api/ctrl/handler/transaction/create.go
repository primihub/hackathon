package transaction

import (
	"encoding/json"
	"fmt"
	"strconv"
	"time"

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

// CreateHandle 创建交易处理方法
func CreateHandle(c *gin.Context) {
	hd := &CreateHandler{}
	handler.Handle(c, hd)
}

// CreateHandler 创建交易处理对象
type CreateHandler struct {
	Req  CreateRequest
	Resp CreateResponse
}

// CreateRequest 创建交易请求
type CreateRequest struct {
	Username          string `json:"username"`
	ProductID         string `json:"productId" binding:"required"`
	Turnover          int    `json:"turnover" binding:"required"`
	TransferorEncoded bool   `json:"transferorEncoded"`
	TransfereeEncoded bool   `json:"transfereeEncoded"`
	CreateTimeEncoded bool   `json:"createTimeEncoded"`
	Policy            string `json:"policy"`
}

// CreateResponse 创建产品返回
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
	// 获取产品信息
	resp, err := fabric.ClientInstance.Query("", "getProduct", fabric.PackArgs([]string{h.Req.ProductID}), nil, nil)
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
	if product.Owner.OriginValue == "***" {
		msg := "product owner encrypted"
		h.SetError(common.ErrorInner, msg)
		seelog.Error(msg)
		return
	}

	// 封装数据
	trasnaction := &chaincode.Transaction{
		ProductID: h.Req.ProductID,
		Transferor: chaincode.Field{
			Encoded:     h.Req.TransferorEncoded,
			OriginValue: product.Owner.OriginValue,
		},
		Transferee: chaincode.Field{
			Encoded:     h.Req.TransfereeEncoded,
			OriginValue: h.Req.Username,
		},
		Turnover: h.Req.Turnover,
		CreateTime: chaincode.Field{
			Encoded:     h.Req.CreateTimeEncoded,
			OriginValue: time.Now().Format(common.TimeFormat),
		},
	}
	// 获取kpabe属性
	kpAttributes, err := util.GetKPAttributes()
	if err != nil {
		msg := fmt.Sprintf("get kp attributes error, %v", err)
		h.SetError(common.ErrorInner, msg)
		seelog.Error(msg)
		return
	}
	// 获取kpabe参数
	params, err := util.GetKPParams()
	if err != nil {
		msg := fmt.Sprintf("get kp params error, %v", err)
		h.SetError(common.ErrorInner, msg)
		seelog.Error(msg)
		return
	}
	// 加密
	if err := crypto.ABEEncrypt([]string{h.Req.Username, product.Owner.OriginValue.(string)}, trasnaction, kpAttributes, params, h.Req.Policy); err != nil {
		msg := fmt.Sprintf("abe encrypt error, %v", err)
		h.SetError(common.ErrorInner, msg)
		seelog.Error(msg)
		return
	}
	var value, cpabeEncrypted string
	if h.Req.Policy != "" {
		value = trasnaction.EncodedValue
		cpabeEncrypted = "Y"
	} else {
		// 序列化
		b, err := json.Marshal(trasnaction)
		if err != nil {
			msg := fmt.Sprintf("marshal Transaction error, %v", err)
			h.SetError(common.ErrorMarshal, msg)
			seelog.Error(msg)
			return
		}
		value = string(b)
		cpabeEncrypted = "N"
	}
	// 调用合约存证
	resp, err = fabric.ClientInstance.Invoke("", "createTransaction", fabric.PackArgs([]string{cpabeEncrypted, value, h.Req.ProductID, strconv.Itoa(h.Req.Turnover)}), nil, nil)
	if err != nil {
		msg := fmt.Sprintf("invoke chaincode error, %v", err)
		h.SetError(common.ErrorInvoke, msg)
		seelog.Error(msg)
		return
	}
	seelog.Infof("tx id: %s, valid code: %d", resp.TransactionID, resp.TxValidationCode)
}
