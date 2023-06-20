package product

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

// UpdateHandle 更新产品处理方法
func UpdateHandle(c *gin.Context) {
	hd := &UpdateHandler{}
	handler.Handle(c, hd)
}

// UpdateHandler 更新产品处理对象
type UpdateHandler struct {
	Req  UpdateRequest
	Resp UpdateResponse
}

// UpdateRequest 创建产品请求
type UpdateRequest struct {
	Username           string `json:"username"`
	ID                 string `json:"id" binding:"required"`
	Name               string `json:"name" binding:"required"`
	NameEncoded        bool   `json:"nameEncoded"`
	Description        string `json:"description"`
	DescriptionEncoded bool   `json:"descriptionEncoded"`
	CreatorEncoded     bool   `json:"creatorEncoded"`
	CreateTimeEncoded  bool   `json:"createTimeEncoded"`
	AssetPackageID     string `json:"assetPackageId" binding:"required"`
	Amount             int    `json:"amount" binding:"required"`
	Policy             string `json:"policy"`
}

// UpdateResponse 更新产品返回
type UpdateResponse struct {
	common.BaseResponse
}

// BindReq 绑定参数
func (h *UpdateHandler) BindReq(c *gin.Context) error {
	if err := c.ShouldBindBodyWith(&h.Req, binding.JSON); err != nil {
		msg := fmt.Sprintf("invalid request, bind error: %v", err)
		seelog.Error(msg)
		h.SetError(common.ErrorPanic, msg)
		return err
	}
	return nil
}

// AfterBindReq 绑定参数后校验参数
func (h *UpdateHandler) AfterBindReq() error {
	return nil
}

// GetResponse 获取返回信息
func (h *UpdateHandler) GetResponse() interface{} {
	return h.Resp
}

// SetError 设置错误信息
func (h *UpdateHandler) SetError(code int, message string) {
	h.Resp.Code = code
	h.Resp.Message = message
}

// SetUsername 设置请求者用户名
func (h *UpdateHandler) SetUsername(username string) {
	h.Req.Username = username
}

// NeedVerifyToken 是否需要验证token
func (h *UpdateHandler) NeedVerifyToken() bool {
	return true
}

// Process 接口处理
func (h *UpdateHandler) Process() {
	// TODO: 判断是否是自己的产品

	// 封装数据
	product := &chaincode.Product{
		ID: h.Req.ID,
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
		Owner: chaincode.Field{
			Encoded:     h.Req.CreatorEncoded,
			OriginValue: h.Req.Username,
		},
		CreateTime: chaincode.Field{
			Encoded:     h.Req.CreateTimeEncoded,
			OriginValue: time.Now().Format(common.TimeFormat),
		},
		Amount:         h.Req.Amount,
		AssetPackageID: h.Req.AssetPackageID,
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
	if err := crypto.ABEEncrypt([]string{h.Req.Username}, product, kpAttributes, params, h.Req.Policy); err != nil {
		msg := fmt.Sprintf("abe encrypt error, %v", err)
		h.SetError(common.ErrorInner, msg)
		seelog.Error(msg)
		return
	}
	var value, cpabeEncrypted string
	if h.Req.Policy != "" {
		value = product.EncodedValue
		cpabeEncrypted = "Y"
	} else {
		// 序列化
		b, err := json.Marshal(product)
		if err != nil {
			msg := fmt.Sprintf("marshal Asset error, %v", err)
			h.SetError(common.ErrorMarshal, msg)
			seelog.Error(msg)
			return
		}
		value = string(b)
		cpabeEncrypted = "N"
	}
	// 调用合约存证
	resp, err := fabric.ClientInstance.Invoke("", "updateProduct", fabric.PackArgs([]string{cpabeEncrypted, value, h.Req.AssetPackageID, strconv.Itoa(h.Req.Amount), h.Req.ID}), nil, nil)
	if err != nil {
		msg := fmt.Sprintf("invoke chaincode error, %v", err)
		h.SetError(common.ErrorInvoke, msg)
		seelog.Error(msg)
		return
	}
	seelog.Infof("tx id: %s, valid code: %d", resp.TransactionID, resp.TxValidationCode)
}
