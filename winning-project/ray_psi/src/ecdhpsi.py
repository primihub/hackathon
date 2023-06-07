import grpc
import sys
sys.path.append("..")
import pickle
import numpy as np
from math import ceil
from src.ecdhpsi_base import PsiBase
# from proto.Server import PsiReceiverService
from src.proto.Server import PsiReceiverService
from src.proto import Client, transport_pb2_grpc, psi_pb2
from src.proto import transport_pb2
from concurrent import futures
# from threading import Thread

class PsiService(object):
    def __init__(self, local_addr_and_port:str, peer_addr_and_port:str, local_rank:int,
                peer_rank:int, version:int, supported_algos:list, algo_params, 
                iterm_num:int,supported_versions:list, curves:list, hash_methods:list, 
                result_to_rank = -1, bucket_num = 1) -> None:
        self.grpc_service = PsiReceiverService(
            version, supported_algos, algo_params, iterm_num,
            supported_versions, curves,
            hash_methods, result_to_rank, bucket_num
        )
        # self.local_addr_and_port = local_addr_and_port
        self.server = self.init_server(local_addr_and_port)
        self.server.start()
        self.client_stub = self.init_client(peer_addr_and_port)
        self.ecdhpsi_base = None
        self.local_rank = local_rank
        self.peer_rank = peer_rank
        self.private_key = None

    def init_server(self, local_addr_and_port:str):
        MAX_MESSAGE_LENGTH = 50 * 1024 * 1024  
        _ONE_DAY_IN_SECONDS = 60 * 60 * 240
        server = grpc.server(futures.ThreadPoolExecutor(max_workers=10),
            options=[('grpc.max_send_message_length', MAX_MESSAGE_LENGTH),
            ('grpc.max_receive_message_length', MAX_MESSAGE_LENGTH)]
        )

        transport_pb2_grpc.add_ReceiverServiceServicer_to_server(
            self.grpc_service, server
        )
        server.add_insecure_port(local_addr_and_port)
        return server
        # try:
        #     while True:
        #         time.sleep(_ONE_DAY_IN_SECONDS)
        # except KeyboardInterrupt:
        #     server.stop(0)
        
    def init_client(self, peer_addr_and_port:str):
        channel = grpc.insecure_channel(peer_addr_and_port)
        stub = transport_pb2_grpc.ReceiverServiceStub(channel)
        return stub
    
    def send_shake_param(self):
        ecdh_psi_params_dict = {
        "supported_versions" : self.grpc_service.supported_versions,
        "curves": self.grpc_service.curves,
        "hash_methods": self.grpc_service.hash_methods,
        "nonce" : None
        }

        hand_shake_parmas_dict = {
        "version": self.grpc_service.verson,
        "supported_algos":self.grpc_service.supported_algos,
        "algo_params": None,
        "item_num": self.grpc_service.iterm_num,
        "bucket_num": self.grpc_service.bucket_num,
        "result_to_rank": self.grpc_service.result_to_rank
        }
        peer_shake_rspinfo = Client.hand_shake(self.client_stub, 
                                          hand_shake_parmas_dict,
                                          ecdh_psi_params_dict)
        return peer_shake_rspinfo

    def send_push(self, data:bytes, index:int, stage:str, 
                  is_last_batch:bool, trans_type:int,
                  bucket_index=1, batch_index=0, count=1):
        """
        data: Data needs to send
        index : The current number of data
        stage: "enc" is first stage, "dual.enc" is second stage
        """
        ecdh_psi_cipher_batch = psi_pb2.EcdhPsiCipherBatch(
            type = stage,
            bucket_index = bucket_index,
            batch_index = batch_index,
            is_last_batch = is_last_batch,
            count = count,
            ciphertext = data
        )
        value = ecdh_psi_cipher_batch.SerializeToString()
        if trans_type == 0:
            transtype = transport_pb2.TransType.MONO 
        else:
            transtype = transport_pb2.TransType.CHUNKED
        chunk_info = transport_pb2.ChunkInfo(message_length = len(data), chunk_offset = 0)
        key = "root:P2P-" + str(index) + ":" + str(self.local_rank)+ "->"+\
                str(self.peer_rank)
        
        push_request = {
            "sender_rank": self.local_rank,
            "key":key,
            "value" : value,
            "trans_type": transtype,
            "chunk_info": chunk_info
        }

        push_response = Client.push(self.client_stub, push_request)
        return push_response
    
    def send_encrypted_id(self, encrypted_id:list, stage:str, batch_size:int):
        batch_num = ceil(len(encrypted_id) / batch_size)
        ind = 0
        if (batch_num == 1):
            trans_type = 0
        else:
            trans_type = 1
        for i in range(batch_num):
            sub_encrypted_id = encrypted_id[ind:ind + batch_size]
            ind += batch_size
            data = pickle.dumps(sub_encrypted_id)
            is_last_batch = False
            if i == (batch_num - 1):
                is_last_batch = True

            push_response = self.send_push(data=data, index=i, stage=stage, 
                        is_last_batch=is_last_batch, trans_type=trans_type,
                        bucket_index=1, batch_index=i, count=len(sub_encrypted_id))
                # deel push_response ......
        return True

    def deel_push_data(self):
        push_data = self.grpc_service.get_push_data()
        data_bytes = push_data["value"]["ciphertext"]
        data_list = pickle.loads(data_bytes)
        is_last_batch = push_data["value"]["is_last_batch"]
        return [data_list,is_last_batch]
    
    def compute_psi(self, id_list:list, batch_size:int):
        # 1、hand shake 
        # For two party, we treat rank 0 (initialtor) as the client(guest) and rank 1
        # as the server(host).
        print('start compute intersection--------\n')
        if self.local_rank == 0:
            peer_shake_rspinfo = self.send_shake_param()
            print("finesd hand shake, PSI params:------\n",peer_shake_rspinfo["algo"])
            if peer_shake_rspinfo["algo"] == "ECDH-PSI":
                # print("----\n",peer_shake_rspinfo["algo_params"]["curve"])
                self.ecdhpsi_base = PsiBase(peer_shake_rspinfo["algo_params"]["curve"])    
        else:
            while True:
                if (self.grpc_service.loacl_psi_params_info):
                    break
              
            print("finesd hand shake, PSI params:------\n",
                  self.grpc_service.loacl_psi_params_info["algo"])
            
            if self.grpc_service.loacl_psi_params_info["algo"] == "ECDH-PSI":
                self.ecdhpsi_base = PsiBase(self.grpc_service.loacl_psi_params_info["algo_params"]["curve"])

        # 2、computes a * h(x_i)
        print("starting to compute a * H(x)------------\n")
        self.private_key = self.ecdhpsi_base.curve.generate_private_keys()
        encrypted_id = self.ecdhpsi_base.id_encrypt2(self.private_key, id_list)

        # 3、sends encrypted_id to the peer
        print("sends encrypted_id to the peer----------\n")
        result1 = self.send_encrypted_id(encrypted_id, "enc", batch_size)
        if(result1):
            print("Successfully sent the first encrypted message--\n")
          
        # 4、receive b * H(y_i) from peer and computes a * (b * H(y_i))
        print("starting to computes a * (b * H(y_i))------\n")
        peer_double_encrypted_id = list()
        while True:
            data_list_i = self.deel_push_data()
            double_encrypted_id_i = self.ecdhpsi_base.double_id_encrypt2(self.private_key,
                                                data_list_i[0])
            peer_double_encrypted_id.extend(double_encrypted_id_i)
            # np.append(peer_double_encrypted_id, double_encrypted_id_i)
            if data_list_i[1]:
                break
    
        # 5、send double encrypted id to peer
        if (self.grpc_service.result_to_rank == -1 or self.grpc_service.result_to_rank != self.local_rank):
            result2 = self.send_encrypted_id(peer_double_encrypted_id, "daul.enc", batch_size)
            if(result2):
                print("Successfully sent the double encrypted message--\n")

        if (self.grpc_service.result_to_rank == -1 or self.grpc_service.result_to_rank == self.local_rank):
            self_double_encrypted_id = list()
            while True:
                data_list_i = self.deel_push_data()
                self_double_encrypted_id.extend(data_list_i[0])
                # np.append(self_double_encrypted_id, data_list_i[0])
                if data_list_i[1]:
                    break
            
            print("starting to compare----------\n")
            intersection_id_list = self.ecdhpsi_base.find_id_by_intersection(
                id_list, self_double_encrypted_id, peer_double_encrypted_id
            )
            return intersection_id_list
        else:
            return []
    

    
