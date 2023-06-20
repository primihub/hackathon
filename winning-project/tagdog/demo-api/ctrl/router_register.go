package ctrl

import (
	"github.com/privacy-protection/demo-api/common"
	"github.com/privacy-protection/demo-api/ctrl/handler/asset"
	assetpackage "github.com/privacy-protection/demo-api/ctrl/handler/asset_package"
	"github.com/privacy-protection/demo-api/ctrl/handler/attribute"
	"github.com/privacy-protection/demo-api/ctrl/handler/key"
	"github.com/privacy-protection/demo-api/ctrl/handler/policy"
	"github.com/privacy-protection/demo-api/ctrl/handler/product"
	"github.com/privacy-protection/demo-api/ctrl/handler/transaction"
	"github.com/privacy-protection/demo-api/ctrl/handler/user"
)

func init() {
	initUserInterfaces()
	initKeyInterfaces()
	initPolicyInterfaces()
	initAttributeInterfaces()
	initAssetPackageInterfaces()
	initAssetInterfaces()
	initProductInterfaces()
	initTransactionInterfaces()
}

func initUserInterfaces() {
	common.RouterRegister.RegisterRouterHandler(common.RouterHandler{
		Path:    "/api/v1/register",
		Method:  "POST",
		Handler: user.RegisterHandle,
	})
	common.RouterRegister.RegisterRouterHandler(common.RouterHandler{
		Path:    "/api/v1/login",
		Method:  "POST",
		Handler: user.LoginHandle,
	})
	common.RouterRegister.RegisterRouterHandler(common.RouterHandler{
		Path:    "/api/v1/getCurrentUser",
		Method:  "POST",
		Handler: user.GetUserHandle,
	})
	common.RouterRegister.RegisterRouterHandler(common.RouterHandler{
		Path:    "/api/v1/getUsers",
		Method:  "POST",
		Handler: user.GetUsersHandle,
	})
}

func initPolicyInterfaces() {
	common.RouterRegister.RegisterRouterHandler(common.RouterHandler{
		Path:    "/api/v1/createPolicy",
		Method:  "POST",
		Handler: policy.CreateHandle,
	})
	common.RouterRegister.RegisterRouterHandler(common.RouterHandler{
		Path:    "/api/v1/getPolicies",
		Method:  "POST",
		Handler: policy.GetHandle,
	})
	common.RouterRegister.RegisterRouterHandler(common.RouterHandler{
		Path:    "/api/v1/deletePolicy",
		Method:  "POST",
		Handler: policy.DeleteHandle,
	})
}

func initAttributeInterfaces() {
	common.RouterRegister.RegisterRouterHandler(common.RouterHandler{
		Path:    "/api/v1/getCPAttributes",
		Method:  "POST",
		Handler: attribute.GetCPAttributesHandle,
	})
	common.RouterRegister.RegisterRouterHandler(common.RouterHandler{
		Path:    "/api/v1/getKPAttributes",
		Method:  "POST",
		Handler: attribute.GetKPAttributesHandle,
	})
}

func initKeyInterfaces() {
	common.RouterRegister.RegisterRouterHandler(common.RouterHandler{
		Path:    "/api/v1/createKey",
		Method:  "POST",
		Handler: key.CreateHandle,
	})
	common.RouterRegister.RegisterRouterHandler(common.RouterHandler{
		Path:    "/api/v1/getKeys",
		Method:  "POST",
		Handler: key.GetHandle,
	})
}

func initAssetPackageInterfaces() {
	common.RouterRegister.RegisterRouterHandler(common.RouterHandler{
		Path:    "/api/v1/createAssetPackage",
		Method:  "POST",
		Handler: assetpackage.CreateHandle,
	})
	common.RouterRegister.RegisterRouterHandler(common.RouterHandler{
		Path:    "/api/v1/getAssetPackages",
		Method:  "POST",
		Handler: assetpackage.GetHandle,
	})
	common.RouterRegister.RegisterRouterHandler(common.RouterHandler{
		Path:    "/api/v1/deleteAssetPackage",
		Method:  "POST",
		Handler: assetpackage.DeleteHandle,
	})
}

func initAssetInterfaces() {
	common.RouterRegister.RegisterRouterHandler(common.RouterHandler{
		Path:    "/api/v1/createAsset",
		Method:  "POST",
		Handler: asset.CreateHandle,
	})
	common.RouterRegister.RegisterRouterHandler(common.RouterHandler{
		Path:    "/api/v1/getAssets",
		Method:  "POST",
		Handler: asset.GetHandle,
	})
	common.RouterRegister.RegisterRouterHandler(common.RouterHandler{
		Path:    "/api/v1/deleteAsset",
		Method:  "POST",
		Handler: asset.DeleteHandle,
	})
}

func initProductInterfaces() {
	common.RouterRegister.RegisterRouterHandler(common.RouterHandler{
		Path:    "/api/v1/createProduct",
		Method:  "POST",
		Handler: product.CreateHandle,
	})
	common.RouterRegister.RegisterRouterHandler(common.RouterHandler{
		Path:    "/api/v1/getProducts",
		Method:  "POST",
		Handler: product.GetHandle,
	})
	common.RouterRegister.RegisterRouterHandler(common.RouterHandler{
		Path:    "/api/v1/deleteProduct",
		Method:  "POST",
		Handler: product.DeleteHandle,
	})
	common.RouterRegister.RegisterRouterHandler(common.RouterHandler{
		Path:    "/api/v1/updateProduct",
		Method:  "POST",
		Handler: product.UpdateHandle,
	})
	common.RouterRegister.RegisterRouterHandler(common.RouterHandler{
		Path:    "/api/v1/confirmProduct",
		Method:  "POST",
		Handler: product.ConfirmHandle,
	})
	common.RouterRegister.RegisterRouterHandler(common.RouterHandler{
		Path:    "/api/v1/auditProduct",
		Method:  "POST",
		Handler: product.AuditHandle,
	})
}

func initTransactionInterfaces() {
	common.RouterRegister.RegisterRouterHandler(common.RouterHandler{
		Path:    "/api/v1/createTransaction",
		Method:  "POST",
		Handler: transaction.CreateHandle,
	})
	common.RouterRegister.RegisterRouterHandler(common.RouterHandler{
		Path:    "/api/v1/getTransactions",
		Method:  "POST",
		Handler: transaction.GetHandle,
	})
}
