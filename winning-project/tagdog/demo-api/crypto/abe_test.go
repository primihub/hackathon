package crypto

import (
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"testing"

	"github.com/google/uuid"
	"github.com/hyperledger/fabric-sdk-go/pkg/crypto/fle"
	"github.com/privacy-protection/demo-api/chaincode"
	"github.com/privacy-protection/demo-api/conf"
	"github.com/stretchr/testify/require"
)

func TestABE(t *testing.T) {
	conf.Conf = &conf.Config{CPABEConf: conf.CPABEConfig{Path: "/root/go/src/github.com/privacy-protection/demo-network/abe-config/cp-abe"}}
	assetPackage := &chaincode.AssetPackage{
		Name: chaincode.Field{
			Encoded:     false,
			OriginValue: "name",
		},
		Description: chaincode.Field{
			Encoded:     true,
			OriginValue: "description",
		},
	}
	kpAttributes := generateKPAttributes(t)
	masterKey, err := fle.Setup()
	require.NoError(t, err)
	key1, err := fle.KeyGen(masterKey, []int{1})
	require.NoError(t, err)
	key4, err := fle.KeyGen(masterKey, []int{4})
	require.NoError(t, err)
	cpKey := generateCPKey(t)
	defer os.Remove(cpKey)

	err = ABEEncrypt([]string{"test"}, assetPackage, kpAttributes, masterKey.Param, "role_admin")
	require.NoError(t, err)
	fmt.Printf("%v\n", assetPackage.EncodedValue)

	err = ABEDecrypt(assetPackage, []*core.Key{key1, key4}, cpKey)
	require.NoError(t, err)
	fmt.Printf("%v\n", assetPackage)
	require.Equal(t, assetPackage.Description.OriginValue, "description")
}

func generateKPAttributes(t *testing.T) []*chaincode.KPAttribute {
	attribute := []*chaincode.KPAttribute{}
	attribute = append(attribute, &chaincode.KPAttribute{
		ID:    1,
		Value: "/v1/assetPackage/name",
		Name:  "资产包名",
	})
	attribute = append(attribute, &chaincode.KPAttribute{
		ID:    2,
		Value: "/v1/assetPackage/description",
		Name:  "资产包描述",
	})
	attribute = append(attribute, &chaincode.KPAttribute{
		ID:    3,
		Value: "/v1/user/admin",
		Name:  "Admin",
	})
	attribute = append(attribute, &chaincode.KPAttribute{
		ID:    4,
		Value: "/v1/user/test",
		Name:  "test",
	})
	return attribute
}

func generateCPKey(t *testing.T) string {
	fileName := filepath.Join(os.TempDir(), uuid.New().String())
	command := fmt.Sprintf("cd %s && oabe_keygen -s CP -i \"username_%s|organization_%s|role_%s\" -o %s", conf.Conf.CPABEConf.Path, "admin", "org1", "admin", fileName)
	b, err := exec.Command("/bin/sh", "-c", command).CombinedOutput()
	require.NoError(t, err)
	fmt.Println(string(b))
	fileName += ".key"
	return fileName
}

func generateKPKeys(t *testing.T) string {
	fileName := filepath.Join(os.TempDir(), uuid.New().String())
	command := fmt.Sprintf("cd %s && oabe_keygen -s KP -i \"username_%s|organization_%s|role_%s\" -o %s", conf.Conf.CPABEConf.Path, "admin", "org1", "admin", fileName)
	b, err := exec.Command("/bin/sh", "-c", command).CombinedOutput()
	require.NoError(t, err)
	fmt.Println(string(b))
	fileName += ".key"
	return fileName
}
