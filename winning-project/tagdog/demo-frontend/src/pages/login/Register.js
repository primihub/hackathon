import { useEffect, useState } from "react";
import { Button, Input, message, Form, Space, Radio } from "antd";
import { useNavigate } from "react-router";
import _ from "lodash";
import { register } from "../../common/apiUtils";
import styles from "./Login.module.css";
import { RoleEnum } from "../../constants/constants";
import { ACCESS_TOKEN, ACCOUNT_INFO } from '../../constants/url';
// import { mockAccountInfo } from "../../constants/mockData";
import { setCookie } from "../../common/utils";
import { switchAccount } from "../../features/account/account";

function isValidEmail({ value }) {
  const rep = /^[a-zA-Z0-9]+@(?:[a-zA-Z0-9]+\.)+[A-Za-z]+$/;
  const patt = new RegExp(rep, "g");
  return patt.test(value);
};

function Register() {
  const navigate = useNavigate();
  const [messageApi, contextHolder] = message.useMessage();
  const [loginType, setLoginType] = useState(1);
  const [showOrganization, setShowOrganization] = useState(true);
  const [nameLabel, setNameLabel] = useState('姓名');
  function gotoHome() {
    navigate('/');
  };
  function gotoLogin() {
    messageApi.info("跳转message测试");
    navigate('/login');
  }

  function onValuesChange(changedValues, allValues) {
    if(allValues.role === RoleEnum.Person) {
      setShowOrganization(true);
    setNameLabel('姓名');
    } else {
      setShowOrganization(pre => !pre);
      setNameLabel('机构名称');
    }
  }

  const [form] = Form.useForm();
  const registerAccount = async (values) => {
    console.log(values);
    // if success
    await register(values)
    .then(response => {
      console.log(response);
      if(response.code === 200) {
        messageApi.info("注册成功！");
        gotoLogin();
      } else {
        throw response.message;
      }
    }).catch(error => {
      console.log(error);
      messageApi.error("Login with account failed!");
    });
    // todo: delete
    messageApi.info("注册成功！");
    gotoLogin();
  };
  
  return (
    <div className={styles.App}>
        {contextHolder}
      <div className={styles.title}>医疗数据搜索系统</div>
      <Form
      layout="vertical"
      form={form}
      initialValues={{
        role: RoleEnum.Person,
      }}
      onValuesChange={onValuesChange}
      onFinish={registerAccount}
      // onValuesChange={}
      style={{
        maxWidth: 600,
      }}
    >
      <Form.Item name="username" label="用户名" rules={[{ required: true, message: '请输入用户名' }]}>
        <Input placeholder="请输入用户名" />
      </Form.Item>
      <Form.Item name="password" label="密码" rules={[{ required: true, message: '请输入密码' }]}>
        <Input.Password placeholder="请输入密码" />
      </Form.Item>
      <Form.Item name="role" label="用户身份" rules={[{ required: true, message: '请输入密码' }]}>
        <Radio.Group>
            <Radio value={RoleEnum.Person}> 个人 </Radio>
            <Radio value={RoleEnum.Institution}> 机构 </Radio>
          </Radio.Group>
      </Form.Item>
      {/* {showOrganization && <Form.Item name="organization" label="所属机构" rules={[{ required: true, message: '请输入用户属性' }]}>
        <Input placeholder="请输入所属机构" />
      </Form.Item>} */}
      <Form.Item name="name" label={nameLabel} rules={[{ required: true, message: '请输入用户属性' }]}>
        <Input placeholder="请输入名称" />
      </Form.Item>
      <Form.Item>
        <Space>
          <Button type="primary" htmlType="submit">注册</Button>
          <Button type="default" onClick={gotoLogin}>去登录</Button>
        </Space>
      </Form.Item>
    </Form>
    </div>
  );
}

export default Register;
