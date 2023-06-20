package user

import (
	"crypto/sha256"
	"encoding/base64"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"os"
	"os/exec"
	"path/filepath"
	"strconv"
	"time"

	"github.com/gin-gonic/gin"
	"github.com/gin-gonic/gin/binding"
	"github.com/golang/protobuf/proto"
	"github.com/google/uuid"
	"github.com/hyperledger/fabric-sdk-go/pkg/crypto/fle"
	"github.com/privacy-protection/demo-api/chaincode"
	"github.com/privacy-protection/demo-api/common"
	"github.com/privacy-protection/demo-api/conf"
	"github.com/privacy-protection/demo-api/crypto"
	"github.com/privacy-protection/demo-api/ctrl/handler"
	"github.com/privacy-protection/demo-api/fabric"
	"github.com/privacy-protection/demo-api/seelog"
)

// RegisterHandle 注册处理方法
func RegisterHandle(c *gin.Context) {
	hd := &RegisterHandler{}
	handler.Handle(c, hd)
}

// RegisterHandler 注册处理对象
type RegisterHandler struct {
	Req  RegisterRequest
	Resp RegisterResponse
}

// RegisterRequest 注册请求
type RegisterRequest struct {
	Username     string `json:"username" binding:"required"`
	Password     string `json:"password" binding:"required"`
	Organization string `json:"organization" binding:"required"`
	Name         string `json:"name" binding:"required"`
	Role         string `json:"role" binding:"required"`
}

// RegisterResponse 注册返回
type RegisterResponse struct {
	common.BaseResponse
}

// BindReq 绑定参数
func (h *RegisterHandler) BindReq(c *gin.Context) error {
	if err := c.ShouldBindBodyWith(&h.Req, binding.JSON); err != nil {
		msg := fmt.Sprintf("invalid request, bind error: %v", err)
		seelog.Error(msg)
		h.SetError(common.ErrorPanic, msg)
		return err
	}
	return nil
}

// AfterBindReq 绑定参数后校验参数
func (h *RegisterHandler) AfterBindReq() error {
	return nil
}

// GetResponse 获取返回信息
func (h *RegisterHandler) GetResponse() interface{} {
	return h.Resp
}

// SetError 设置错误信息
func (h *RegisterHandler) SetError(code int, message string) {
	h.Resp.Code = code
	h.Resp.Message = message
}

// SetUsername 设置请求者用户名
func (h *RegisterHandler) SetUsername(username string) {
}

// NeedVerifyToken 是否需要验证token
func (h *RegisterHandler) NeedVerifyToken() bool {
	return false
}

// Process 接口处理
func (h *RegisterHandler) Process() {
	// 用户对象封装
	hash := sha256.New()
	hash.Sum([]byte(h.Req.Password))
	hp := hash.Sum(nil)
	user := chaincode.User{
		Username:     h.Req.Username,
		Password:     base64.StdEncoding.EncodeToString(hp),
		Organization: h.Req.Organization,
		Name:         h.Req.Name,
		Role:         h.Req.Role,
	}
	userBytes, err := json.Marshal(user)
	if err != nil {
		msg := fmt.Sprintf("marshal User error, %v", err)
		h.SetError(common.ErrorMarshal, msg)
		seelog.Error(msg)
		return
	}

	// 生成cpabe密钥
	fileName := filepath.Join(os.TempDir(), uuid.New().String())
	command := fmt.Sprintf("cd %s && oabe_keygen -s CP -i \"username_%s|organization_%s|role_%s\" -o %s", conf.Conf.CPABEConf.Path, user.Username, user.Organization, user.Role, fileName)
	seelog.Infof("command: %s", command)
	b, err := exec.Command("/bin/sh", "-c", command).CombinedOutput()
	seelog.Infof("command execute output: %s", string(b))
	if err != nil {
		msg := fmt.Sprintf("execute command error, %v", err)
		h.SetError(common.ErrorExecuteCommand, msg)
		seelog.Error(msg)
		return
	}
	fileName += ".key"
	defer os.Remove(fileName)
	b, err = ioutil.ReadFile(fileName)
	if err != nil {
		msg := fmt.Sprintf("read file, %v", err)
		h.SetError(common.ErrorInner, msg)
		seelog.Error(msg)
		return
	}
	encryptedKey, err := crypto.AESEncrypt([]byte(b))
	if err != nil {
		msg := fmt.Sprintf("aes encrypt error, %v", err)
		h.SetError(common.ErrorInner, msg)
		seelog.Error(msg)
		return
	}

	// 执行合约注册接口
	resp, err := fabric.ClientInstance.Invoke("", "register", fabric.PackArgs([]string{string(userBytes), base64.StdEncoding.EncodeToString(encryptedKey)}), nil, nil)
	if err != nil {
		msg := fmt.Sprintf("invoke chaincode error, %v", err)
		h.SetError(common.ErrorInvoke, msg)
		seelog.Error(msg)
		return
	}
	seelog.Infof("tx id: %s, valid code: %d", resp.TransactionID, resp.TxValidationCode)

	// 创建kpabe密钥
	kpAttributeId, err := strconv.Atoi(string(resp.Responses[0].Response.Payload))
	if err != nil {
		msg := fmt.Sprintf("string to int error, %v", err)
		h.SetError(common.ErrorInner, msg)
		seelog.Error(msg)
		return
	}
	key, err := fle.KeyGen(conf.Conf.KPABEConf.MasterKey, []int{kpAttributeId})
	if err != nil {
		msg := fmt.Sprintf("generate kp key error, %v", err)
		h.SetError(common.ErrorInner, msg)
		seelog.Error(msg)
		return
	}
	b, err = proto.Marshal(key)
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
	kpKey := &chaincode.KPKey{
		Name:        h.Req.Name,
		Description: h.Req.Name,
		Creator:     h.Req.Username,
		Owner:       h.Req.Username,
		CreateTime:  time.Now().Format(common.TimeFormat),
		Attributes: []chaincode.KPAttribute{{
			ID: kpAttributeId,
		}},
		Value: base64.StdEncoding.EncodeToString(b),
	}
	b, err = json.Marshal(kpKey)
	if err != nil {
		msg := fmt.Sprintf("marshal KPKey error, %v", err)
		h.SetError(common.ErrorMarshal, msg)
		seelog.Error(msg)
		return
	}

	// 执行合约创建密钥接口
	resp, err = fabric.ClientInstance.Invoke("", "createKey", fabric.PackArgs([]string{string(b)}), nil, nil)
	if err != nil {
		msg := fmt.Sprintf("invoke chaincode error, %v", err)
		h.SetError(common.ErrorInvoke, msg)
		seelog.Error(msg)
		return
	}
	seelog.Infof("tx id: %s, valid code: %d", resp.TransactionID, resp.TxValidationCode)
}
