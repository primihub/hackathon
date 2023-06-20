/*
Copyright IBM Corp. All Rights Reserved.

SPDX-License-Identifier: Apache-2.0
*/

package pvtdatapolicy

import (
	"math"
	"testing"

	"github.com/hyperledger/fabric/core/common/privdata"
	"github.com/hyperledger/fabric/core/ledger/pvtdatapolicy/mock"
	"github.com/hyperledger/fabric/protos/common"
	"github.com/stretchr/testify/assert"
)

func TestBTLPolicy(t *testing.T) {
	btlPolicy := testutilSampleBTLPolicy()
	btl1, err := btlPolicy.GetBTL("ns1", "coll1")
	assert.NoError(t, err)
	assert.Equal(t, uint64(100), btl1)

	btl2, err := btlPolicy.GetBTL("ns1", "coll2")
	assert.NoError(t, err)
	assert.Equal(t, uint64(200), btl2)

	btl3, err := btlPolicy.GetBTL("ns1", "coll3")
	assert.NoError(t, err)
	assert.Equal(t, defaultBTL, btl3)

	_, err = btlPolicy.GetBTL("ns1", "coll4")
	_, ok := err.(privdata.NoSuchCollectionError)
	assert.True(t, ok)
}

func TestExpiringBlock(t *testing.T) {
	btlPolicy := testutilSampleBTLPolicy()
	expiringBlk, err := btlPolicy.GetExpiringBlock("ns1", "coll1", 50)
	assert.NoError(t, err)
	assert.Equal(t, uint64(151), expiringBlk)

	expiringBlk, err = btlPolicy.GetExpiringBlock("ns1", "coll2", 50)
	assert.NoError(t, err)
	assert.Equal(t, uint64(251), expiringBlk)

	expiringBlk, err = btlPolicy.GetExpiringBlock("ns1", "coll3", 50)
	assert.NoError(t, err)
	assert.Equal(t, uint64(math.MaxUint64), expiringBlk)

	expiringBlk, err = btlPolicy.GetExpiringBlock("ns1", "coll4", 50)
	_, ok := err.(privdata.NoSuchCollectionError)
	assert.True(t, ok)
}

func testutilSampleBTLPolicy() BTLPolicy {
	ccInfoRetriever := &mock.CollectionInfoProvider{}
	ccInfoRetriever.CollectionInfoStub = func(ccName, collName string) (*common.StaticCollectionConfig, error) {
		collConfig := &common.StaticCollectionConfig{}
		var err error
		switch collName {
		case "coll1":
			collConfig.BlockToLive = 100
		case "coll2":
			collConfig.BlockToLive = 200
		case "coll3":
			collConfig.BlockToLive = 0
		default:
			err = privdata.NoSuchCollectionError{}
		}
		return collConfig, err
	}
	return ConstructBTLPolicy(ccInfoRetriever)
}
