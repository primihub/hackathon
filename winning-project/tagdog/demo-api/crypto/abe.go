package crypto

import (
	"encoding/base64"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"os"
	"os/exec"
	"path/filepath"
	"reflect"
	"strconv"

	"github.com/google/uuid"
	"github.com/privacy-protection/demo-api/chaincode"
	"github.com/privacy-protection/demo-api/conf"
	"github.com/privacy-protection/demo-api/seelog"
)

type nullStruct struct{}

// ABEEncrypt abe加密，policy不为空是需将object的其他字段清空
func ABEEncrypt(usernames []string, object interface{}, kpAttribytes []*chaincode.KPAttribute, policy string) error {
	// kpabe加密
	numberField := reflect.ValueOf(object).Elem().NumField()
	for i := 0; i < numberField; i++ {
		v := reflect.ValueOf(object).Elem().Field(i)
		if v.Type() == reflect.ValueOf(chaincode.Field{}).Type() {
			if v.FieldByName("Encoded").Bool() {
				attributeName := reflect.TypeOf(object).Elem().Field(i).Tag.Get("cpabe")
				fields := []int{
					getCPAttributeIDByName(attributeName, kpAttribytes),
					getCPAttributeIDByName("/v1/user/admin", kpAttribytes),
				}
				for _, username := range usernames {
					fields = append(fields, getCPAttributeIDByName(fmt.Sprintf("/v1/user/%s", username), kpAttribytes))
				}
				b, err := json.Marshal(v.FieldByName("OriginValue").Interface())
				if err != nil {
					return fmt.Errorf("marshal field error, %v", err)
				}
				//拼接kpabe属性
				attributesLen := len(fields)
				attributesStr := ""

				attributesStr += strconv.Itoa(fields[0])
				for i := 1; i < attributesLen; i++ {
					attributesStr += "|" + strconv.Itoa(fields[i])
				}
				// 将原文写入文件中
				textFileName := filepath.Join(os.TempDir(), uuid.New().String()+".txt")
				if err := ioutil.WriteFile(textFileName, b, 0644); err != nil {
					return fmt.Errorf("write file error, %v", err)
				}
				defer os.Remove(textFileName)
				ciphertextFileName := filepath.Join(os.TempDir(), uuid.New().String()+".txt")
				// 执行命令加密
				command := fmt.Sprintf("cd %s && oabe_enc -s KP -e \"%s\" -i %s -o %s", conf.Conf.CPABEConf.Path, attributesStr, textFileName, ciphertextFileName)
				seelog.Infof("command: %s", command)
				b, err = exec.Command("/bin/sh", "-c", command).CombinedOutput()
				seelog.Infof("command execute output: %s", string(b))
				if err != nil {
					return fmt.Errorf("execute command error, %v", err)
				}
				ciphertextFileName += ".kpabe"
				defer os.Remove(ciphertextFileName)
				// 从文件中读取密文
				encryptedData, err := ioutil.ReadFile(ciphertextFileName)
				if err != nil {
					return fmt.Errorf("read file error, %v", err)
				}
				reflect.ValueOf(object).Elem().FieldByName("EncodedValue").Set(reflect.ValueOf(string(encryptedData)))
				if err != nil {
					return fmt.Errorf("fle encrypt error, %v", err)
				}
				v.FieldByName("OriginValue").Set(reflect.ValueOf(nullStruct{}))
				v.FieldByName("EncodedValue").Set(reflect.ValueOf(base64.StdEncoding.EncodeToString(encryptedData)))
			}
		}
	}
	// cpabe加密
	if policy != "" {
		b, err := json.Marshal(object)
		if err != nil {
			return fmt.Errorf("marshal object error, %v", err)
		}
		// 将原文写入文件中
		textFileName := filepath.Join(os.TempDir(), uuid.New().String()+".txt")
		if err := ioutil.WriteFile(textFileName, b, 0644); err != nil {
			return fmt.Errorf("write file error, %v", err)
		}
		defer os.Remove(textFileName)
		ciphertextFileName := filepath.Join(os.TempDir(), uuid.New().String()+".txt")
		// 修改策略
		policy = fmt.Sprintf("(%s) | username_admin", policy)
		for _, username := range usernames {
			policy = fmt.Sprintf("%s | username_%s", policy, username)
		}
		// 执行命令加密
		command := fmt.Sprintf("cd %s && oabe_enc -s CP -e \"%s\" -i %s -o %s", conf.Conf.CPABEConf.Path, policy, textFileName, ciphertextFileName)
		seelog.Infof("command: %s", command)
		b, err = exec.Command("/bin/sh", "-c", command).CombinedOutput()
		seelog.Infof("command execute output: %s", string(b))
		if err != nil {
			return fmt.Errorf("execute command error, %v", err)
		}
		ciphertextFileName += ".cpabe"
		defer os.Remove(ciphertextFileName)
		// 从文件中读取密文
		b, err = ioutil.ReadFile(ciphertextFileName)
		if err != nil {
			return fmt.Errorf("read file error, %v", err)
		}
		reflect.ValueOf(object).Elem().FieldByName("EncodedValue").Set(reflect.ValueOf(string(b)))
	}
	return nil
}

// ABEDecrypt abe解密
func ABEDecrypt(object interface{}, kpKeys []string, cpKey string) error {
	numberField := reflect.ValueOf(object).Elem().NumField()

	// cpabe解密
	s := reflect.ValueOf(object).Elem().FieldByName("EncodedValue").String()
	if s != "" {
		// 填充"***"字符
		for i := 0; i < numberField; i++ {
			v := reflect.ValueOf(object).Elem().Field(i)
			if v.Type() == reflect.ValueOf(chaincode.Field{}).Type() {
				v.FieldByName("OriginValue").Set(reflect.ValueOf("***"))
			}
		}
		// 将密文写入文件中
		ciphertextFileName := filepath.Join(os.TempDir(), uuid.New().String()+".txt.cpabe")
		if err := ioutil.WriteFile(ciphertextFileName, []byte(s), 0644); err != nil {
			return fmt.Errorf("write file error, %v", err)
		}
		defer os.Remove(ciphertextFileName)
		textFileName := filepath.Join(os.TempDir(), uuid.New().String()+".txt")
		defer os.Remove(textFileName)
		// 执行命令解密
		command := fmt.Sprintf("cd %s && oabe_dec -s CP -k %s -i %s -o %s", conf.Conf.CPABEConf.Path, cpKey, ciphertextFileName, textFileName)
		seelog.Infof("command: %s", command)
		b, err := exec.Command("/bin/sh", "-c", command).CombinedOutput()
		seelog.Infof("command execute output: %s", string(b))
		if err != nil {
			return fmt.Errorf("execute command error, %v", err)
		}
		// 从文件中读取密文
		b, err = ioutil.ReadFile(textFileName)
		if err != nil {
			return fmt.Errorf("read file error, %v", err)
		}
		if err := json.Unmarshal(b, object); err != nil {
			return fmt.Errorf("unmarshal object error, %v", err)
		}
	}
	for i := 0; i < numberField; i++ {
		v := reflect.ValueOf(object).Elem().Field(i)
		if v.Type() == reflect.ValueOf(chaincode.Field{}).Type() {
			if v.FieldByName("Encoded").Bool() {
				v.FieldByName("OriginValue").Set(reflect.ValueOf("***"))
				d := v.FieldByName("EncodedValue").String()
				encryptedData, err := base64.StdEncoding.DecodeString(d)
				if err != nil {
					return fmt.Errorf("base64 decode error, %v", err)
				}
				for _, k := range kpKeys {
					// 将密文写入文件中
					ciphertextFileName := filepath.Join(os.TempDir(), uuid.New().String()+".txt.kpabe")
					if err := ioutil.WriteFile(ciphertextFileName, []byte(s), 0644); err != nil {
						return fmt.Errorf("write file error, %v", err)
					}
					defer os.Remove(ciphertextFileName)
					textFileName := filepath.Join(os.TempDir(), uuid.New().String()+".txt")
					defer os.Remove(textFileName)
					// 执行命令解密
					command := fmt.Sprintf("cd %s && oabe_dec -s KP -k %s -i %s -o %s", conf.Conf.CPABEConf.Path, k, ciphertextFileName, textFileName)
					seelog.Infof("command: %s", command)
					b, err := exec.Command("/bin/sh", "-c", command).CombinedOutput()
					seelog.Infof("command execute output: %s", string(b))
					if err != nil {
						return fmt.Errorf("execute command error, %v", err)
					}
					// 从文件中读取密文
					b, err = ioutil.ReadFile(textFileName)
					if err != nil {
						return fmt.Errorf("read file error, %v", err)
					}
					if err := json.Unmarshal(b, object); err != nil {
						return fmt.Errorf("unmarshal object error, %v", err)
					}
					var fieldValue interface{}
					if err := json.Unmarshal(b, &fieldValue); err != nil {
						return fmt.Errorf("unmarshal field error, %v", err)
					}
					v.FieldByName("OriginValue").Set(reflect.ValueOf(fieldValue))
					v.FieldByName("EncodedValue").Set(reflect.ValueOf(""))
				}
			}
		}
	}
	return nil
}

func getCPAttributeIDByName(name string, kpAttribytes []*chaincode.KPAttribute) int {
	for _, attribute := range kpAttribytes {
		if attribute.Value == name {
			return attribute.ID
		}
	}
	seelog.Errorf("invalid attribute name: %s", name)
	return 0
}
