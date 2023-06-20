/*
Copyright IBM Corp. All Rights Reserved.

SPDX-License-Identifier: Apache-2.0
*/

package privdata

import (
	"github.com/gogo/protobuf/proto"
	"github.com/hyperledger/fabric/common/cauthdsl"
	"github.com/hyperledger/fabric/common/policies"
	"github.com/hyperledger/fabric/msp"
	"github.com/hyperledger/fabric/protos/common"
	mspp "github.com/hyperledger/fabric/protos/msp"
	"github.com/pkg/errors"
)

// getPolicy creates a new policy from the policy envelope. It will return an error if the envelope has invalid policy config.
// Some caller (e.g., MembershipProvider.AsMemberOf) may drop the error and treat it as a RejectAll policy.
// In the future, we must revisit the callers if this method will return different types of errors.
func getPolicy(collectionPolicyConfig *common.CollectionPolicyConfig, deserializer msp.IdentityDeserializer) (policies.Policy, error) {
	if collectionPolicyConfig == nil {
		return nil, errors.New("collection policy config is nil")
	}
	accessPolicyEnvelope := collectionPolicyConfig.GetSignaturePolicy()
	if accessPolicyEnvelope == nil {
		return nil, errors.New("collection config access policy is nil")
	}
	// create access policy from the envelope

	pp := cauthdsl.EnvelopeBasedPolicyProvider{Deserializer: deserializer}
	accessPolicy, err := pp.NewPolicy(accessPolicyEnvelope)
	if err != nil {
		return nil, errors.WithMessage(err, "failed constructing policy object out of collection policy config")
	}

	return accessPolicy, nil
}

// getMemberOrgs returns a list containing member orgs from a list of MSPPrincipals, it will skip identities it fails to process
func getMemberOrgs(identities []*mspp.MSPPrincipal, deserializer msp.IdentityDeserializer) []string {
	memberOrgs := []string{}

	// get member org MSP IDs from the envelope
	for _, principal := range identities {
		switch principal.PrincipalClassification {
		case mspp.MSPPrincipal_ROLE:
			// Principal contains the msp role
			mspRole := &mspp.MSPRole{}
			err := proto.Unmarshal(principal.Principal, mspRole)
			if err == nil {
				memberOrgs = append(memberOrgs, mspRole.MspIdentifier)
			}
		case mspp.MSPPrincipal_IDENTITY:
			principalId, err := deserializer.DeserializeIdentity(principal.Principal)
			if err == nil {
				memberOrgs = append(memberOrgs, principalId.GetMSPIdentifier())
			}
		case mspp.MSPPrincipal_ORGANIZATION_UNIT:
			OU := &mspp.OrganizationUnit{}
			err := proto.Unmarshal(principal.Principal, OU)
			if err == nil {
				memberOrgs = append(memberOrgs, OU.MspIdentifier)
			}
		}
	}
	return memberOrgs
}
