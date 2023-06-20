import { useEffect, useState } from "react";
import { useSelector } from "react-redux";
import { message, Table, Tag, Modal, Form, Input, Button, Upload } from "antd";
import { dataColumns, userData, getDataColumns } from "./datas";
import { authDataStatusCode, authDataStatusString, RoleEnum } from '../../constants/constants';
import _ from "lodash";
import styles from "./Home.module.css";
import { addNewMR, getUMRByKeyword, queryUserGrant } from "../../common/apiUtils";
import { setSelectionRange } from "@testing-library/user-event/dist/utils";
import { FILE_BASE_URL } from "../../constants/url";

function Access() {
  const [messageApi, contextHolder] = message.useMessage();
  const [form] = Form.useForm();
  const account = useSelector((state) => state.account.value);
  const { Search } = Input;
  const [data, setData] = useState([]);
  const [fullTextMR, setFullTextMR] = useState("");
  const [mrUploadModalOpen, setMrUploadModalOpen] = useState(false);
  const [mrInfoModalOpen, setMrInfoModalOpen] = useState(false);
  const [requestAuthedId, setRequestAuthedId] = useState(-1);
  const [userVerifyCode, setUserVerifyCode] = useState("");
  const [isGranted, setIsGranted] = useState(false);

  async function fetchData(keyword) {
    await getUMRByKeyword({ keyword: keyword })
      .then(response => {
        console.log(response);
        if (response.code === 200) {
          setData(response.UMRs);
        } else {
          throw response.message;
        }
      }).catch(error => {
        console.log(error);
        messageApi.error("getUMRByKeyword error");
        setData(userData);
      });
  }

  useEffect(() => {
    fetchData(account.username);
  }, []);

  const onSearch = (value) => {
    console.log(value);
    fetchData(value);
  };

  const userAuthVerify = async () => {
    setMrUploadModalOpen(false);
    await queryUserGrant({ username: userVerifyCode })
      .then(response => {
        setMrUploadModalOpen(false);
        if (response.code === 200) {
          setIsGranted(response.grant);
          if (response.grant) {
            setMrInfoModalOpen(true);
          }
        } else {
          throw response.message;
        }
      })
      .catch(error => {
        console.log(error);
        setMrInfoModalOpen(true); // todo: delete
        messageApi.error("queryUserGrant error!");
      });
  };

  const normFile = (e) => {
    console.log('Upload event:', e);
    if (Array.isArray(e)) {
      return e;
    }
    return e?.fileList;
  };

  const addNewRecord = async (values) => {
    console.log(values);
    const realValues = {
      textMR: values.textMR,
      username: userVerifyCode,
      appendixMR: _.map(values.upload, (file) => {
        return FILE_BASE_URL + '/' +file.name;
      }),
    };
    console.log(realValues);
    await addNewMR(realValues)
    .then(response => {
      console.log(response);
    })
    .catch(error => {
      console.log(error);
    });
    setMrInfoModalOpen(false);
  };

  const requestGrant = () => {
    console.log(requestAuthedId);
  };

  return (
    <div className={styles.App}>
      {contextHolder}
      <div className={styles.operation}>
      <Search placeholder="input search text" allowClear onSearch={onSearch} style={{ width: 800, margin: 20 }} />
      {account.role === RoleEnum.Institution &&<Button type="primary" onClick={() => setMrUploadModalOpen(true)}>病历上传</Button>}
      </div>
      <div>此表格为本机构/本人有权限查看的数据</div>
      <Table columns={getDataColumns({ showAllTextMR: (text) => setFullTextMR(text), requestAuth: (id) => setRequestAuthedId(id) })} dataSource={data} />
      <Modal title="是否请求用户授权查看？" open={!!(requestAuthedId > 0)} onOk={requestGrant} onCancel={() => setRequestAuthedId(-1)}>
        用户ID：{requestAuthedId}
      </Modal>
      <Modal title="病历文本" open={fullTextMR !== ""} onOk={() => setFullTextMR("")} onCancel={() => setFullTextMR("")} footer={null}>
        {fullTextMR}
      </Modal>
      <Modal title="请输入用户名" open={mrUploadModalOpen} onOk={userAuthVerify} onCancel={() => setMrUploadModalOpen(false)}>
        <Input className={styles.userVerifyInput} value={userVerifyCode} onChange={(e) => { setUserVerifyCode(e.target.value); }} />
      </Modal>
      <Modal title="添加用户病历" open={mrInfoModalOpen} onCancel={() => setMrInfoModalOpen(false)} footer={null}>
        <Form
          form={form}
          initialValues={{
            layout: "vertical",
          }}
          onFinish={addNewRecord}
        >
          <Form.Item name="textMR" label="病历文本" rules={[{ required: true, message: '请输入病历文本' }]}>
            <Input.TextArea />
          </Form.Item>
          <Form.Item
            name="upload"
            label="病历附件"
            valuePropName="fileList"
            getValueFromEvent={normFile}
          >
            <Upload name="logo" action="/upload.do">
              <Button>点击上传附件</Button>
            </Upload>
          </Form.Item>
          <Form.Item>
            <Button type="primary" htmlType="submit">提交</Button>
          </Form.Item>
        </Form>
      </Modal>
    </div>
  );
}

export default Access;
