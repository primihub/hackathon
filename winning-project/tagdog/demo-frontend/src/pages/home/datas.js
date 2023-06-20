import { Tag, Space, Input } from "antd";
import { RoleEnum } from "../../constants/constants";

export const userInfo = {
  name: "hui test",
  attribution: "something",
  role: RoleEnum.Person,
};
export const homeTabItems = [{
  key: 'userInfo',
  label: '个人信息',
}, {
  key: 'dataAccess',
  label: '数据访问',
}, {
  key: 'dataAuthenrization',
  label: '数据授权'
}];

export function getDataColumns({ showAllTextMR, requestAuth }) {
  return [{
    title: 'ID',
    dataIndex: 'id',
    key: 'MRid'
  }, {
    title: '病历文本',
    dataIndex: 'textMR',
    key: 'textMR',
    width: 600,
    ellipsis: true,
    render: (_, { id, textMR, plaintext }) => {
      return plaintext ?
      (<span onClick={() => showAllTextMR(textMR)}>{textMR}</span>) :
      (<span onClick={() => requestAuth(id)}>***</span>);
    },
    filters: [
      {
        text: '明文',
        value: true,
      },
      {
        text: '密文',
        value: false,
      },
    ],
    filterMode: 'tree',
    filterSearch: true,
    onFilter: (value, record) => record.plaintext === value,
    width: '30%',
  }, {
    title: '上传时间',
    dataIndex: 'date',
    key: 'date',
    render: (date) => <span>{date ? date.toLocaleDateString() : 'no date'}</span>,
  }, {
    title: '上传方机构名称',
    dataIndex: 'medicalInst',
    key: 'medicalInst',
  }, {
    title: '病历附件',
    dataIndex: 'appendixMR',
    key: 'appendixMR',
    render: (_, { appendixMR }) => (
      <>
        {appendixMR.map((url, i) => {
          let color = url.length > 10 ? 'geekblue' : 'green';
          return (
            <Tag color={color} key={url}>
              <a href={url}>附件 {i+1}</a>
            </Tag>
          );
        })}
      </>
    ),
  }
];
}
export const dataColumns = [{
    title: 'ID',
    dataIndex: 'id',
    key: 'MRid'
  }, {
    title: '病历文本',
    dataIndex: 'textMR',
    key: 'textMR',
    width: 600,
    ellipsis: true,
    render: (_, { textMR, plaintext }) => {
      return plaintext ?
      (<span>{textMR}</span>) :
      (<span>***</span>)
    },
  }, {
    title: '上传时间',
    dataIndex: 'date',
    key: 'date',
    render: (date) => <span>{date ? date.toLocaleDateString() : 'no date'}</span>,
  }, {
    title: '上传方机构名称',
    dataIndex: 'medicalInst',
    key: 'medicalInst',
  }, {
    title: '病历附件',
    dataIndex: 'appendixMR',
    key: 'appendixMR',
    render: (_, { appendixMR }) => (
      <>
        {appendixMR.map((url, i) => {
          let color = url.length > 10 ? 'geekblue' : 'green';
          return (
            <Tag color={color} key={url}>
              <a href={url}>附件 {i+1}</a>
            </Tag>
          );
        })}
      </>
    ),
  }
];
export const userData = [{
  key: '1',
  id: '1',
  date: new Date(),
  textMR: 'this is text-MR test this is text-MR test this is text-MR test this is text-MR test this is text-MR test this is text-MR test this is text-MR test this is text-MR test this is text-MR test this is text-MR test this is text-MR test this is text-MR test this is text-MR test this is text-M this is text-MR test this is text-MR test this is text-MR test this is text-MR testR test',
  userInfo: 'sdfsfd666xfsdfshash',
  medicalInst: '解放军医院',
  appendixMR: ['http://baidu.com', 'www.weibo.com', 'http://baidu.com', 'www.weibo.com'],
  plaintext: true,
}, {
  key: '2',
  id: '2',
  date: new Date(),
  textMR: 'this is text-MR test',
  userInfo: 'sdfsfd666xfsdfshash',
  medicalInst: '解放军医院',
  appendixMR: ['http://baidu.com', 'www.weibo.com', 'http://baidu.com', 'www.weibo.com'],
  plaintext: false,
}, {
  key: '3',
  id: '3',
  date: new Date(),
  textMR: 'this is text-MR test',
  userInfo: 'sdfsfd666xfsdfshash',
  medicalInst: '解放军医院',
  appendixMR: ['http://baidu.com', 'www.weibo.com', 'http://baidu.com', 'www.weibo.com'],
  plaintext: true,
}, {
  key: '4',
  id: '4',
  date: new Date(),
  textMR: 'this is text-MR test',
  userInfo: 'sdfsfd666xfsdfshash',
  medicalInst: '解放军医院',
  appendixMR: ['http://baidu.com', 'www.weibo.com', 'http://baidu.com', 'www.weibo.com'],
  plaintext: false,
}];

export const mockWaitAuthenrizeData = [{
  id: 1,
  source: '解放军医院',
  content: '解放军医院查看病历',
  status: 'WaitingAuth',
}, {
  id: 1,
  source: '北医三院',
  content: '北医三院调档',
  status: 'Authed',
}];
