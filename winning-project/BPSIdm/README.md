BPSIdm：Blockchain-Based Privacy-Preserving Self-Sovereign Identity Management
==============================================================================
Project Background
-----------------------

Digital identity management is an important field of research by experts and scholars at home and abroad in the field of network security, and the existing digital identity management framework is mainly centralized and consorted, and there are generally risks of single point of failure and privacy information leakage. BPSIdm is a Blockchain-based privacy-preserving self-sovereign identity management project,it aims to realize the decentralized self-sovereign management of users' digital identities.</br>

Main Ideas
----------

1.User's self-sovereign</br>
The so-called user's self-sovereign means that users have full ownership and control over their own Digital identity data.One way to achieve this is that the identity data is completely stored in the user's digital wallet,and only the user knows that external service providers can only use the user's identity data but not see it. After realizing the user's self-sovereign identity,users can effectively prevent the abuse of Digital identity and the disclosure of private information.</br>
2.Uniqueness of identity</br>
Compared with the traditional Digital identity management scheme, self-sovereign identity management pays more attention to the uniqueness of the user's Digital identity. This feature means that users have only one Digital identity on the Internet, and only one Digital identity user can access all Internet services. In this way, users will no longer have to worry about frequently forgetting the login user name or password of an online service. A crucial issue that needs to be addressed in order to achieve this is the interoperability between different service providers and digital wallets where users store their identities. It is necessary to ensure that the service provider can establish a connection with the user's digital wallet for identity authentication and other operations when accessing the service. Based on the current development situation, the popularization of blockchain technology is necessary.</br>

Environmental Configuration
--------------------------
truffle v5.8.1</br>
Node.js v14.21.3</br>
npm v6.14.18</br>
ganache v7.8.0</br>

Project Compile
---------------
```truffle compile```</br>
```truffle migrate```</br>

Project Deploy
-------------
```node sever.js```</br>
```npm run start```</br>
