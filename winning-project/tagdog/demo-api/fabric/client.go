package fabric

import (
	"fmt"

	"github.com/hyperledger/fabric-sdk-go/pkg/client/channel"
	"github.com/hyperledger/fabric-sdk-go/pkg/core/config"
	"github.com/hyperledger/fabric-sdk-go/pkg/fabsdk"
	"github.com/privacy-protection/demo-api/conf"
	"github.com/privacy-protection/demo-api/seelog"
)

// ClientInstance 客户端实例
var ClientInstance *Client

// Client 客户端
type Client struct {
	sdk           *fabsdk.FabricSDK
	channelClient *channel.Client
}

// InitClient 客户端初始化
func InitClient() error {
	sdk, err := fabsdk.New(config.FromFile(conf.DefaultSDKConfigFile))
	if err != nil {
		return fmt.Errorf("new fabric sdk client error, %v", err)
	}
	cc := sdk.ChannelContext(defaultChannelID, fabsdk.WithUser(defaultUser), fabsdk.WithOrg(defaultOrg))
	channelClient, err := channel.New(cc)
	if err != nil {
		return fmt.Errorf("new channel client error, %v", err)
	}
	ClientInstance = &Client{
		sdk:           sdk,
		channelClient: channelClient,
	}
	return nil
}

// Invoke 执行合约
func (c *Client) Invoke(
	chaincodeID string,
	fcn string,
	args [][]byte,
	transientMap map[string][]byte,
	peers []string,
) (*channel.Response, error) {
	if chaincodeID == "" {
		chaincodeID = defaultChaincodeID
	}
	req := channel.Request{
		ChaincodeID:  chaincodeID,
		Fcn:          fcn,
		Args:         args,
		TransientMap: transientMap,
	}
	if peers == nil || len(peers) == 0 {
		peers = []string{peer0Org1, peer0Org2, peer0Org3}
	}
	resp, err := c.channelClient.Execute(req, channel.WithTargetEndpoints(peers...))
	if err != nil {
		return nil, fmt.Errorf("execute error, %v", err)
	}
	return &resp, nil
}

// Query 查询合约
func (c *Client) Query(
	chaincodeID string,
	fcn string,
	args [][]byte,
	transientMap map[string][]byte,
	peers []string,
) (*channel.Response, error) {
	if chaincodeID == "" {
		chaincodeID = defaultChaincodeID
	}
	req := channel.Request{
		ChaincodeID:  chaincodeID,
		Fcn:          fcn,
		Args:         args,
		TransientMap: transientMap,
	}
	if peers == nil || len(peers) == 0 {
		peers = []string{peer0Org1, peer0Org2, peer0Org3}
	}
	resp, err := c.channelClient.Query(req, channel.WithTargetEndpoints(peers...))
	if err != nil {
		return nil, fmt.Errorf("execute error, %v", err)
	}
	seelog.Infof("query chaincode response: %s", string(resp.Responses[0].Response.Payload))
	return &resp, nil
}
