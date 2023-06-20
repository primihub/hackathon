import { Button, Input, message, Form, Space } from "antd";
import { useNavigate } from "react-router";
import { useDispatch } from "react-redux";
import _ from "lodash";
import styles from "./Login.module.css";
import { login } from "../../common/apiUtils";
import { setCookie } from "../../common/utils";
import { ACCESS_TOKEN, ACCOUNT_INFO } from "../../constants/url";
import { switchAccount } from "../../features/account/account";

function Login() {
  const navigate = useNavigate();
  const dispatch = useDispatch();
  const [messageApi, contextHolder] = message.useMessage();
  function gotoHome() {
    navigate('/');
  };
  function gotoRegister() {
    messageApi.info("跳转message测试");
    navigate('/register');
  }

  const [form] = Form.useForm();
  const loginAccount = async (values) => {
    console.log(values);
    await login(values)
    .then(response => {
      console.log(response);
      if(response.code === 200) {
        setCookie(ACCESS_TOKEN, response.token, 60);
        localStorage.setItem(ACCESS_TOKEN, response.token);
        const newAccount = { username: values.username };
        localStorage.setItem(ACCOUNT_INFO, newAccount);
        dispatch(switchAccount({ account: newAccount }));
        messageApi.info("注册成功！");
        gotoHome();
      } else {
        throw response.message;
      }
    }).catch(error => {
      console.log(error);
      messageApi.error("Login with account failed!");
    });
    
    // if success
    messageApi.info("登录成功！");
    gotoHome();    
  };
  
  return (
    <div className={styles.App}>
      {contextHolder}
      <div className={styles.title}>医疗数据搜索系统</div>
      <Form
      layout="vertical"
      form={form}
      initialValues={{
        layout: "vertical",
      }}
      onFinish={loginAccount}
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
      <Form.Item>
        <Space>
          <Button type="primary" htmlType="submit">登录</Button>
          <Button type="default" onClick={gotoRegister}>去注册</Button>
        </Space>
      </Form.Item>
    </Form>
    </div>
  );
}

export default Login;
