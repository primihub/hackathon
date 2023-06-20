package chaincode

// User 用户信息
type User struct {
	Username     string `json:"username,omitempty"`
	Password     string `json:"password,omitempty"`
	Organization string `json:"organization,omitempty"`
	Name         string `json:"name,omitempty"`
	Role         string `json:"role,omitempty"`
}

// AssetPackage 资产包
type AssetPackage struct {
	ID           string    `json:"id,omitempty"`
	Name         Field     `json:"name,omitempty" cpabe:"/v1/assetPackage/name"`
	Description  Field     `json:"description,omitempty" cpabe:"/v1/assetPackage/description"`
	Creator      Field     `json:"creator,omitempty" cpabe:"/v1/assetPackage/creator"`
	CreateTime   Field     `json:"createTime,omitempty" cpabe:"/v1/assetPackage/createTime"`
	TotalAsset   int       `json:"totalAsset,omitempty"`
	Assets       []Asset   `json:"assets,omitempty"`
	Products     []Product `json:"products,omitempty"`
	EncodedValue string    `json:"encodedValue,omitempty"`
}

// Asset 资产信息
type Asset struct {
	ID             string `json:"id,omitempty"`
	Name           Field  `json:"name,omitempty" cpabe:"/v1/asset/name"`
	Description    Field  `json:"description,omitempty" cpabe:"/v1/asset/description"`
	Creator        Field  `json:"creator,omitempty" cpabe:"/v1/asset/creator"`
	CreateTime     Field  `json:"createTime,omitempty" cpabe:"/v1/asset/createTime"`
	AssetPackageID string `json:"assetPackageId,omitempty"`
	EncodedValue   string `json:"encodedValue,omitempty"`
}

// Product 产品信息
type Product struct {
	ID             string `json:"id,omitempty"`
	Name           Field  `json:"name,omitempty" cpabe:"/v1/product/name"`
	Description    Field  `json:"description,omitempty" cpabe:"/v1/product/description"`
	Creator        Field  `json:"creator,omitempty" cpabe:"/v1/product/creator"`
	Owner          Field  `json:"owner,omitempty" cpabe:"/v1/product/owner"`
	CreateTime     Field  `json:"createTime,omitempty" cpabe:"/v1/product/createTime"`
	Amount         int    `json:"amount,omitempty"`
	Status         int    `json:"status,omitempty"`
	AssetPackageID string `json:"assetPackageId,omitempty"`
	EncodedValue   string `json:"encodedValue,omitempty"`
}

// Transaction 交易信息
type Transaction struct {
	ID           string `json:"id,omitempty"`
	ProductID    string `json:"productId,omitempty"`
	Transferor   Field  `json:"transferor,omitempty" cpabe:"/v1/transaction/transferor"`
	Transferee   Field  `json:"transferee,omitempty" cpabe:"/v1/transaction/transferee"`
	Turnover     int    `json:"turnover,omitempty"`
	CreateTime   Field  `json:"createTime,omitempty" cpabe:"/v1/transaction/createTime"`
	EncodedValue string `json:"encodedValue,omitempty"`
}

// Field 字段级加密字段
type Field struct {
	Encoded      bool        `json:"encoded,omitempty"`
	OriginValue  interface{} `json:"originValue,omitempty"`
	EncodedValue string      `json:"encodedValue,omitempty"`
}

// CPAttribute cpabe属性
type CPAttribute struct {
	ID     int      `json:"id,omitempty"`
	Field  string   `json:"field,omitempty"`
	Name   string   `json:"name,omitempty"`
	Type   int      `json:"type,omitempty"`
	Values []string `json:"values,omitempty"`
}

// KPAttribute kpabe属性
type KPAttribute struct {
	ID    int    `json:"id,omitempty"`
	Value string `json:"value,omitempty"`
	Name  string `json:"name,omitempty"`
}

// Policy 策略
type Policy struct {
	ID          string `json:"id,omitempty"`
	Description string `json:"description,omitempty"`
	Creator     string `json:"creator,omitempty"`
	Value       string `json:"value,omitempty"`
	CreateTime  string `json:"createTime,omitempty"`
}

// KPKey 密钥
type KPKey struct {
	ID          string        `json:"id,omitempty"`
	Name        string        `json:"name,omitempty"`
	Description string        `json:"description,omitempty"`
	Creator     string        `json:"creator,omitempty"`
	Owner       string        `json:"owner,omitempty"`
	CreateTime  string        `json:"createTime,omitempty"`
	Attributes  []KPAttribute `json:"attributes,omitempty"`
	Value       string        `json:"value,omitempty"`
}
