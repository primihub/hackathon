const express = require('express');
const bodyParser = require('body-parser');
const Web3 = require('web3');
const EthrDID = require('ethr-did');
const DIDRegistryContract = require('ethr-did-registry').default;
const Resolver = require('did-resolver').Resolver;
const getResolver = require('ethr-did-resolver').getResolver;

const app = express();
app.use(bodyParser.json());

const web3 = new Web3(new Web3.providers.HttpProvider("http://localhost:8545"));
const registryAddress = '0xe78A0F7E598Cc8b0Bb87894B0F60dD2a88d6a8Ab';
const registryAbi= require('./build/contracts/EthereumDIDRegistry.json').abi;
const registryContract = new web3.eth.Contract(registryAbi,registryAddress);

app.post('/register', async (req, res) => {
 try{
  const { address, privateKey } = req.body;

  const did = new EthrDID({
    address,
    privateKey,
    registry: registryContract,
    provider: web3.currentProvider
  });
  const resolver = new Resolver(getResolver(registryContract, getNetId(web3)));

  const didDocument = await did.signDIDDocument();
  await registryContract.registerAttribute(did.id, 'did_document', didDocument);
  res.status(200).json({message: 'DID registered successfully'});
 }catch(error){
      console.error(error.message);
      res.status(500).json({ message: 'Something went wrong. Please try again later.' });
   } 
  const result = {
    message: `DID registered successfully: ${did.id}`
  };
  res.json(result);
});

app.listen(3000, () => {
  console.log('Server started at http://localhost:3000');
});

function getNetId(web3) {
  return web3.eth.net.getId().then(netId => netId.toString());
}

