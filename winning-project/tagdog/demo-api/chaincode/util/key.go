package util

import (
	"encoding/base64"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"os"
	"path/filepath"

	"github.com/golang/protobuf/proto"
	"github.com/google/uuid"
	"github.com/privacy-protection/demo-api/chaincode"
	"github.com/privacy-protection/demo-api/crypto"
	"github.com/privacy-protection/demo-api/fabric"
)

// GetCPKey 获取cpabe密钥
func GetCPKey(username string) (string, error) {
	resp, err := fabric.ClientInstance.Query("", "getKey", fabric.PackArgs([]string{username}), nil, nil)
	if err != nil {
		return "", fmt.Errorf("chaincode query error, %v", err)
	}
	s := string(resp.Responses[0].Response.Payload)
	b, err := base64.StdEncoding.DecodeString(s)
	if err != nil {
		return "", fmt.Errorf("base64 decode error, %v", err)
	}
	b, err = crypto.AESDecrypt(b)
	if err != nil {
		return "", fmt.Errorf("aes decrypt error, %v", err)
	}

	// 将密钥写入文件中
	fileName := filepath.Join(os.TempDir(), uuid.New().String()+".key")
	if err := ioutil.WriteFile(fileName, b, 0644); err != nil {
		return "", fmt.Errorf("write file error, %v", err)
	}
	return fileName, nil
}

// GetKPKeys 获取kpabe密钥
func GetKPKeys(username string) ([]*core.Key, error) {
	resp, err := fabric.ClientInstance.Query("", "getKeys", fabric.PackArgs([]string{username}), nil, nil)
	if err != nil {
		return nil, fmt.Errorf("chaincode query error, %v", err)
	}
	s := string(resp.Responses[0].Response.Payload)
	b, err := base64.StdEncoding.DecodeString(s)
	if err != nil {
		return "", fmt.Errorf("base64 decode error, %v", err)
	}
	b, err = crypto.AESDecrypt(b)
	if err != nil {
		return "", fmt.Errorf("aes decrypt error, %v", err)
	}

	// 将密钥写入文件中
	fileName := filepath.Join(os.TempDir(), uuid.New().String()+".key")
	if err := ioutil.WriteFile(fileName, b, 0644); err != nil {
		return "", fmt.Errorf("write file error, %v", err)
	}
	return fileName, nil

}
