
 const DigitalIdentity = artifacts.require("EthereumDIDRegistry");
   
   module.exports = function (deployer) {
     deployer.deploy(DigitalIdentity);
   };
   

