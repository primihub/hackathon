package assetpackage

import (
	"encoding/json"
	"fmt"
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

// CreateHandle 创建资产包处理方法
func CreateHandle(c *gin.Context) {
	hd := &CreateHandler{}
	handler.Handle(c, hd)
}

// CreateHandler 创建资产包处理对象
type CreateHandler struct {
	Req  CreateRequest
	Resp CreateResponse
}

// CreateRequest 创建资产包请求
type CreateRequest struct {
	Username           string `json:"username"`
	Name               string `json:"name" binding:"required"`
	NameEncoded        bool   `json:"nameEncoded"`
	Description        string `json:"description"`
	DescriptionEncoded bool   `json:"descriptionEncoded"`
	CreatorEncoded     bool   `json:"creatorEncoded"`
	CreateTimeEncoded  bool   `json:"createTimeEncoded"`
	Policy             string `json:"policy"`
}

// CreateResponse 创建资产包返回
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
	assetPackage := &chaincode.AssetPackage{
		Name: chaincode.Field{
			Encoded:     h.Req.NameEncoded,
			OriginValue: h.Req.Name,
		},
		Description: chaincode.Field{
			Encoded:     h.Req.DescriptionEncoded,
			OriginValue: h.Req.Description,
		},
		Creator: chaincode.Field{
			Encoded:     h.Req.CreatorEncoded,
			OriginValue: h.Req.Username,
		},
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
	if err := crypto.ABEEncrypt([]string{h.Req.Username}, assetPackage, kpAttributes, params, h.Req.Policy); err != nil {
		msg := fmt.Sprintf("abe encrypt error, %v", err)
		h.SetError(common.ErrorInner, msg)
		seelog.Error(msg)
		return
	}
	var value, cpabeEncrypted string
	if h.Req.Policy != "" {
		value = assetPackage.EncodedValue
		cpabeEncrypted = "Y"
	} else {
		// 序列化
		b, err := json.Marshal(assetPackage)
		if err != nil {
			msg := fmt.Sprintf("marshal AssetPackage error, %v", err)
			h.SetError(common.ErrorMarshal, msg)
			seelog.Error(msg)
			return
		}
		value = string(b)
		cpabeEncrypted = "N"
	}
	// 调用合约存证
	resp, err := fabric.ClientInstance.Invoke("", "createAssetPackage", fabric.PackArgs([]string{cpabeEncrypted, value}), nil, nil)
	if err != nil {
		msg := fmt.Sprintf("invoke chaincode error, %v", err)
		h.SetError(common.ErrorInvoke, msg)
		seelog.Error(msg)
		return
	}
	seelog.Infof("tx id: %s, valid code: %d", resp.TransactionID, resp.TxValidationCode)
}
