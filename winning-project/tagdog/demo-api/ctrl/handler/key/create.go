package key

import (
	"encoding/base64"
	"encoding/json"
	"fmt"
	"time"

	"github.com/gin-gonic/gin"
	"github.com/gin-gonic/gin/binding"
	"github.com/golang/protobuf/proto"
	"github.com/privacy-protection/demo-api/chaincode"
	"github.com/privacy-protection/demo-api/common"
	"github.com/privacy-protection/demo-api/conf"
	"github.com/privacy-protection/demo-api/crypto"
	"github.com/privacy-protection/demo-api/ctrl/handler"
	"github.com/privacy-protection/demo-api/fabric"
	"github.com/privacy-protection/demo-api/seelog"
	"github.com/privacy-protection/fabric-sdk-go/pkg/crypto/fle"
)

// CreateHandle 创建kpabe密钥处理方法
func CreateHandle(c *gin.Context) {
	hd := &CreateHandler{}
	handler.Handle(c, hd)
}

// CreateHandler 创建kpabe密钥处理对象
type CreateHandler struct {
	Req  CreateRequest
	Resp CreateResponse
}

// CreateRequest 创建kpabe密钥请求
type CreateRequest struct {
	Username     string `json:"username"`
	Name         string `json:"name" binding:"required"`
	Description  string `json:"description"`
	Owner        string `json:"owner"  binding:"required"`
	AttributeIds []int  `json:"attributeIds" binding:"required"`
}

// CreateResponse 创建kpabe密钥返回
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
	// 判断是否是管理员
	if h.Req.Username != "admin" {
		msg := fmt.Sprintf("no user [admin], got [%s]", h.Req.Username)
		h.SetError(common.ErrorInner, msg)
		seelog.Error(msg)
		return
	}
	// 创建kpabe密钥
	key, err := fle.KeyGen(conf.Conf.KPABEConf.MasterKey, h.Req.AttributeIds)
	if err != nil {
		msg := fmt.Sprintf("generate kp key error, %v", err)
		h.SetError(common.ErrorInner, msg)
		seelog.Error(msg)
		return
	}
	b, err := proto.Marshal(key)
	if err != nil {
		msg := fmt.Sprintf("marshal Key error, %v", err)
		h.SetError(common.ErrorMarshal, msg)
		seelog.Error(msg)
		return
	}
	b, err = crypto.AESEncrypt(b)
	if err != nil {
		msg := fmt.Sprintf("aes encrypt error, %v", err)
		h.SetError(common.ErrorInner, msg)
		seelog.Error(msg)
		return
	}
	// 封装数据
	kpKey := &chaincode.KPKey{
		Name:        h.Req.Name,
		Description: h.Req.Description,
		Creator:     h.Req.Username,
		Owner:       h.Req.Owner,
		CreateTime:  time.Now().Format(common.TimeFormat),
		Attributes:  []chaincode.KPAttribute{},
		Value:       base64.StdEncoding.EncodeToString(b),
	}
	for _, attributeID := range h.Req.AttributeIds {
		kpKey.Attributes = append(kpKey.Attributes, chaincode.KPAttribute{
			ID: attributeID,
		})
	}
	b, err = json.Marshal(kpKey)
	if err != nil {
		msg := fmt.Sprintf("marshal KPKey error, %v", err)
		h.SetError(common.ErrorMarshal, msg)
		seelog.Error(msg)
		return
	}
	// 调用合约存证
	resp, err := fabric.ClientInstance.Invoke("", "createKey", fabric.PackArgs([]string{string(b)}), nil, nil)
	if err != nil {
		msg := fmt.Sprintf("invoke chaincode error, %v", err)
		h.SetError(common.ErrorInvoke, msg)
		seelog.Error(msg)
		return
	}
	seelog.Infof("tx id: %s, valid code: %d", resp.TransactionID, resp.TxValidationCode)
}
