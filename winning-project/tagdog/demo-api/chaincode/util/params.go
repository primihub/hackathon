package util

import (
	"encoding/pem"
	"fmt"

	"github.com/golang/protobuf/proto"
	"github.com/privacy-protection/demo-api/fabric"
	"github.com/privacy-protection/kp-abe/core"
)

// GetKPParams 获取kpabe参数
func GetKPParams() (*core.Params, error) {
	resp, err := fabric.ClientInstance.Query("", "getKPParams", fabric.PackArgs([]string{}), nil, nil)
	if err != nil {
		return nil, fmt.Errorf("chaincode query error, %v", err)
	}
	b, _ := pem.Decode(resp.Responses[0].Response.Payload)
	params := &core.Params{}
	if err := proto.Unmarshal(b.Bytes, params); err != nil {
		return nil, fmt.Errorf("unmarshal Params error, %v", err)
	}
	return params, nil
}
