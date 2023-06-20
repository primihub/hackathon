import { useEffect, useState } from "react";
import _ from "lodash";
import { message, Table, Tag, Modal, Form, Input, Button } from "antd";
import { authDataStatusCode, authDataStatusString } from '../../constants/constants';
import {grantMR} from '../../common/apiUtils';
import styles from "./Home.module.css";
import { mockWaitAuthenrizeData } from "./datas";

function Auth() {
  const [messageApi, contextHolder] = message.useMessage();
  const [openAuthModal, setOpenAuthModal] = useState(false);
  const [waitAuthenrizeData, setWaitAuthenrizeData] = useState([]);
  const [form] = Form.useForm();

  const fetchGrantData = async () => {
    setWaitAuthenrizeData(mockWaitAuthenrizeData);
  };

  useEffect(() => {
    fetchGrantData();
  }, []);

  const handleCancel = () => {
    setOpenAuthModal(false);
  }
  const toGrant = async (record) => {
    console.log(record);
    await grantMR({ username: record.username })
    .then(response => {
      if(response.code === 200) {
        messageApi.info("授权成功！");
        fetchGrantData();
      } else {
        throw "response code is not 200";
      }
    })
    .catch(error => {
      console.log(error);
      messageApi.error("授权失败！");
    });
    setOpenAuthModal(false);
  };

  const waitAuthenrizeColumns = [{
    title: '来源',
    dataIndex: 'source',
    key: 'username',
  }, {
    title: '内容',
    dataIndex: 'content',
    key: 'content',
  }, {
    title: '状态',
    dataIndex: 'status',
    key: 'status',
    render: (_, { status }) => {
      console.log(status);
      return (
        <Tag color={status === "WaitingAuth" ? "yellow" : "green"}>{authDataStatusString[status]}</Tag>
      );
    },
  }, {
    title: '操作',
    dataIndex: 'operate',
    key: 'operate',
    render: (_, record) => {
      return  record.status === "WaitingAuth" ? (<div>
        <a onClick={() => setOpenAuthModal(true)}>去授权</a>
        <Modal title="请求信息" open={openAuthModal} okText="确认授权" onOk={() => {toGrant(record);}} onCancel={handleCancel}>
        <Form
              labelCol={{ span: 4 }}
              wrapperCol={{ span: 14 }}
              layout="horizontal"
              style={{ maxWidth: 600 }}
            >
              <Form.Item label="来源">
                {record.source}
              </Form.Item>
              <Form.Item label="内容">
                {record.content}
              </Form.Item>
            </Form>
        </Modal>
      </div>
      ) : null;
    },
  }];

  const toGrantInst = async (values) => {
    console.log(values);
    await grantMR(values)
    .then(response => {
      if(response.code === 200) {
        messageApi.info("授权成功！");
        fetchGrantData();
      } else {
        throw "response code is not 200";
      }
    })
    .catch(error => {
      console.log(error);
      messageApi.error("授权失败！");
    });
    setOpenAuthModal(false);
  };
  
  return (
    <div className={styles.App}>
      {contextHolder}
      {/* <div className={styles.statement}>此页面中展示了需要用户授权的数据，点击<strong>操作 {">"} 去授权</strong>将个人数据授权给他人。</div>
      <Table columns={waitAuthenrizeColumns} dataSource={waitAuthenrizeData} /> */}
      <Form
      form={form}
      onFinish={toGrantInst}
      style={{
        margin: 20,
        maxWidth: 600,
      }}
    >
      <Form.Item name="username" label="医疗机构名称" rules={[{ required: true, message: '请输入医疗机构名称' }]}>
        <Input />
      </Form.Item>
      <Form.Item>
        <Button type="primary" htmlType="submit">授权</Button>
      </Form.Item>
    </Form>
    </div>
  );
}

export default Auth;
