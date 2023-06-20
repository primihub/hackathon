package util

import (
	"encoding/json"
	"fmt"

	"github.com/privacy-protection/demo-api/chaincode"
	"github.com/privacy-protection/demo-api/fabric"
)

// GetCPAttributes 获取cpabe属性
func GetCPAttributes() ([]*chaincode.CPAttribute, error) {
	resp, err := fabric.ClientInstance.Query("", "getCPAttributes", fabric.PackArgs([]string{}), nil, nil)
	if err != nil {
		return nil, fmt.Errorf("chaincode query error, %v", err)
	}
	attributes := []*chaincode.CPAttribute{}
	if err := json.Unmarshal(resp.Responses[0].Response.Payload, &attributes); err != nil {
		return nil, fmt.Errorf("unmarshal CPAttribute error, %v", err)
	}
	return attributes, nil
}

// GetKPAttributes 获取kpabe属性
func GetKPAttributes() ([]*chaincode.KPAttribute, error) {
	resp, err := fabric.ClientInstance.Query("", "getKPAttributes", fabric.PackArgs([]string{}), nil, nil)
	if err != nil {
		return nil, fmt.Errorf("chaincode query error, %v", err)
	}
	attributes := []*chaincode.KPAttribute{}
	if err := json.Unmarshal(resp.Responses[0].Response.Payload, &attributes); err != nil {
		return nil, fmt.Errorf("unmarshal KPAttribute error, %v", err)
	}
	return attributes, nil
}
