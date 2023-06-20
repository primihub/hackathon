package asset

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

// GetHandle 获取资产处理方法
func GetHandle(c *gin.Context) {
	hd := &GetHandler{}
	handler.Handle(c, hd)
}

// GetHandler 获取资产处理对象
type GetHandler struct {
	Req  GetRequest
	Resp GetResponse
}

// GetRequest 获取资产包请求
type GetRequest struct {
	Username string `json:"username"`
	Current  int    `json:"current" binding:"required"`
	PageSize int    `json:"pageSize" binding:"required"`
	Filter   bool   `json:"filter"`
}

// GetResponse 获取资产返回
type GetResponse struct {
	common.BaseResponse
	Total    int     `json:"total"`
	PageSize int     `json:"pageSize"`
	Current  int     `json:"current"`
	Data     []Asset `json:"data"`
}

// Asset 资产信息
type Asset struct {
	ID               string      `json:"id"`
	Name             interface{} `json:"name"`
	Description      interface{} `json:"description"`
	Creator          interface{} `json:"creator"`
	CreateTime       interface{} `json:"createTime"`
	AssetPackageID   string      `json:"assetPackageId"`
	AssetPackageName interface{} `json:"assetPackageName"`
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
	// 获取所有资产包
	resp, err := fabric.ClientInstance.Query("", "getAssets", fabric.PackArgs([]string{}), nil, nil)
	if err != nil {
		msg := fmt.Sprintf("chaincode query error, %v", err)
		h.SetError(common.ErrorQuery, msg)
		seelog.Error(msg)
		return
	}
	assets := []*chaincode.Asset{}
	if err := json.Unmarshal(resp.Responses[0].Response.Payload, &assets); err != nil {
		msg := fmt.Sprintf("unmarshal Asset error, %v", err)
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
	for _, asset := range assets {
		if err := crypto.ABEDecrypt(asset, kpKeys, cpKey); err != nil {
			seelog.Errorf("aes decrypt error, %v", err)
		}
	}
	// 过滤
	if h.Req.Filter {
		filterAssets := []*chaincode.Asset{}
		for _, asset := range assets {
			if asset.Creator.OriginValue == h.Req.Username {
				filterAssets = append(filterAssets, asset)
			}
		}
		assets = filterAssets
	}
	h.Resp.Total = len(assets)
	h.Resp.PageSize = h.Req.PageSize
	h.Resp.Current = h.Req.Current
	end := h.Resp.Current * h.Req.PageSize
	if h.Resp.Total < end {
		end = h.Resp.Total
	}
	h.Resp.Data = make([]Asset, 0)
	for i := (h.Resp.Current - 1) * h.Req.PageSize; i < end; i++ {
		a := Asset{
			ID:             assets[i].ID,
			Name:           assets[i].Name.OriginValue,
			Description:    assets[i].Description.OriginValue,
			Creator:        assets[i].Creator.OriginValue,
			CreateTime:     assets[i].CreateTime.OriginValue,
			AssetPackageID: assets[i].AssetPackageID,
		}

		// 获取对应的资产包信息
		resp, err := fabric.ClientInstance.Query("", "getAssetPackage", fabric.PackArgs([]string{assets[i].AssetPackageID}), nil, nil)
		if err != nil {
			msg := fmt.Sprintf("chaincode query error, %v", err)
			h.SetError(common.ErrorQuery, msg)
			seelog.Error(msg)
			return
		}
		assetPackage := &chaincode.AssetPackage{}
		if err := json.Unmarshal(resp.Responses[0].Response.Payload, assetPackage); err != nil {
			msg := fmt.Sprintf("unmarshal AssetPackage error, %v", err)
			h.SetError(common.ErrorQuery, msg)
			seelog.Error(msg)
			return
		}
		if err := crypto.ABEDecrypt(assetPackage, kpKeys, cpKey); err != nil {
			seelog.Errorf("aes decrypt error, %v", err)
		}

		a.AssetPackageName = assetPackage.Name.OriginValue
		h.Resp.Data = append(h.Resp.Data, a)
	}
}
