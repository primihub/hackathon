import styles from "./Home.module.css";
import _ from "lodash";
import React, { useState, useEffect } from "react";
import { useNavigate } from "react-router";
import { message, Layout, Menu, Form, Button } from "antd";
import { homeTabItems } from "./datas";
import Auth from "./grant";
import { RoleEnum } from "../../constants/constants";
import { ACCESS_TOKEN, ACCOUNT_INFO } from "../../constants/url";
import { getCurrentUser } from "../../common/apiUtils";
import { switchAccount } from "../../features/account/account";
import { useDispatch } from "react-redux";
import Access from "./data-access";

function MedicalSearch() {
  const navigate = useNavigate();
  const dispatch = useDispatch();
  function gotoLogin() {
    navigate('/login');
  }
  const [messageApi, contextHolder] = message.useMessage();
  const { Header, Footer, Sider, Content } = Layout;
  const [currentTab, setCurrentTab] = useState({
    key: 'userInfo',
    label: '个人信息',
  });
  const [userInfo, setUserInfo] = useState({});
  async function fetchUser() {
    const accinfo = JSON.parse(localStorage.getItem(ACCOUNT_INFO));
    await getCurrentUser({username: accinfo.username})
    .then(response => {
      console.log(response);
      if(response.code === 200 && response.user.username === accinfo.username) {
        setUserInfo(response.user);
        localStorage.setItem(ACCOUNT_INFO, JSON.stringify(response.user));
        dispatch(switchAccount({ account: response.user }));
      } else {
        throw response.message;
      }
    }).catch(error => {
      console.log(error);
      messageApi.error("getCurrentUser error");
      // todo: delete
      const mockUser = {
        username : 'hui test mock', //用户名
        "name" : 'name mock',
        "role" : 0,
      };
      setUserInfo(mockUser);
      localStorage.setItem(ACCOUNT_INFO, JSON.stringify(mockUser));
      dispatch(switchAccount({ account: mockUser }));
    });
  }

  useEffect(() => {
    if(!localStorage.getItem(ACCESS_TOKEN)) {
      // todo: delete
      localStorage.setItem(ACCESS_TOKEN, 'aaaa');
      localStorage.setItem(ACCOUNT_INFO, JSON.stringify({ username: 'hui test' }));
      // todo: 这是原本的代码，之后要恢复
      // gotoLogin();
      // return;
    }
    console.log(localStorage.getItem(ACCESS_TOKEN));
    fetchUser();
  }, []);

  const changeTab = (e) => {
    const selectedItem = _.filter(homeTabItems, {key: e.key})[0];
    setCurrentTab({
      key: e.key,
      label: selectedItem.label,
    });
  };

  const headerStyle = {
    textAlign: 'center',
    color: '#fff',
    height: 64,
    paddingInline: 50,
    lineHeight: '64px',
    backgroundColor: '#7dbcea',
  };
  const siderStyle = {
    textAlign: 'center',
    lineHeight: '120px',
    color: '#fff',
    backgroundColor: '#ffffff',
  };

  const logout = () => {
    localStorage.setItem(ACCESS_TOKEN, '');
    localStorage.setItem(ACCOUNT_INFO, {});
    gotoLogin();
  }
 
  return (
    <div className={styles.App}>
      {contextHolder}
      <div className={styles.title}>医疗数据搜索系统</div>
      <Layout>
      <Sider style={siderStyle}>
        <Menu style={{ height: '100%' }}
          defaultSelectedKeys={['userInfo']}
          mode="inline"
          items={homeTabItems} onClick={changeTab}
        />
      </Sider>
      <Layout>
        <Header style={headerStyle}>
          {currentTab.label}
        </Header>
        <Content>
          {currentTab.key === "userInfo" && 
          <div className={styles.userInfo}>
            <Form
              labelCol={{ span: 4 }}
              wrapperCol={{ span: 14 }}
              layout="horizontal"
              style={{ maxWidth: 600 }}
            >
              <Form.Item label="用户名">
                {userInfo.username}
              </Form.Item>
              <Form.Item label={userInfo.role === RoleEnum.Person ? '姓名' : '机构名称'}>
                {userInfo.name}
              </Form.Item>
              <Form.Item label="用户身份">
                {userInfo.role === RoleEnum.Person ? '个人' : '机构'}
              </Form.Item>
              <Form.Item>
                <Button onClick={logout}>退出登录</Button>
              </Form.Item>
            </Form>
          </div>}
          {currentTab.key === "dataAccess" && <Access />}
          {userInfo.role === RoleEnum.Person && currentTab.key === "dataAuthenrization" && 
          <Auth />}
        </Content>
      </Layout>
    </Layout>
    </div>
  )
}

export default MedicalSearch;
