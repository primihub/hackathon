from utils.federated_utils import train, evaluate

def start_server(model, train_data, test_data):
    # 初始化模型参数
    global_model = model

    # 迭代训练和聚合更新
    for epoch in range(num_epochs):
        # 在每个客户端进行训练
        for client_data in train_data:
            client_model = train(global_model, client_data)

            # 将更新的模型参数聚合到全局模型
            global_model = aggregate(global_model, client_model)

        # 在服务器端评估模型性能
        accuracy = evaluate(global_model, test_data)
        print(f"Epoch {epoch+1}: Accuracy = {accuracy}")

    # 保存最终的全局模型
    save_model(global_model)
