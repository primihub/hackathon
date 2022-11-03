<template>
  <div class="main">
    <NavBar />
    <div class="banner">
      <div class="container">
        <div class="button" @click="signup"></div>
      </div>
    </div>
    <b-container>
    <section class="intro" id="intro">
      <div class="section-title">
        <span class="title">活动背景</span>
        <span class="background">Background</span>
      </div>
      <div class="intro-desc">
        <p class="summary text-left">我国相继颁布施行的《网络安全法》、《数据安全法》、《个人信息保护法》，三部法律
        共同组成数据保护领域的“三驾马车”，构筑了一张新时代的数据安全防护网。同期，国务院办公厅在《要素市场化配置综合改革试点总体方案》里提出建立健全数据流通交易规则，探索<strong class="active-color">“原始数据不出域、数据可用不可见”</strong>的交易范式。隐私计算作为数据产业的核心基础设施，其全球市场规模有望突破数万亿。</p>
        <p class="summary text-left"><strong class="active-color">OpenMPC联合CSDN在2022年程序员节开展隐私计算 Hackathon。</strong>本大赛旨在通过竞赛方式，促进隐私计算技术应用与创新，促进更多的数据拥有方和数据使用方通过隐私计算发挥数据价值，促进数字产业与隐私计算结合应用，为打造数据要素流通高效协同的关键基础设施贡献科创力量，促进数据要素化创新应用、技术实践、培育优秀人才。</p>
      </div>
    </section>
    <section id="system">
      <div class="section-title">
        <span class="title">活动赛制</span>
        <span class="background">Format</span>
      </div>
      <div class="system-intro text-left">
        <dl>
          <dt>1.赛事启动</dt>
          <dd>
            <p>第一届隐私计算 Hackathon 在1024程序员节启动。</p>
          </dd>
        </dl>
        <dl>
          <dt>2.参赛对象</dt>
          <dd>
            <ul>
              <li>大赛面向社会各界开放，不限年龄、国籍，高校、科研院所、企业从业人员均可报名参赛。</li>
              <li>报名条件：参赛人员以个人或者团队形式报名参赛，一名参赛人员在一个赛道中仅允许出现1次，但可以参加多个赛道。可以组团参加，每支参赛团队人数不超过5人。</li>
            </ul>
          </dd>
        </dl>
        <dl>
          <dt>3.赛制说明</dt>
          <dd>
            <p class="summary text-left">
              本次比赛，推荐所有参赛团队使用<a class="nocolor" href="https://github.com/primihub/primihub" target="_blank">PrimiHub</a>开源平台进行开发，但不限制。期待通过本场大赛，为隐私计算技术的推进和应用的发展提供新思路。
            </p>
            <p class="summary text-left">
            <a class="nocolor" href="https://github.com/primihub/primihub" target="_blank">PrimiHub</a> 是基于安全多方计算、联邦学习、同态加密、可信计算等的隐私计算技术，结合区块链等研发的开源隐私计算应用平台。
              <a class="nocolor" href="https://github.com/primihub/primihub" target="_blank">PrimiHub</a> 秉承保护数据在应用过程中的隐私安全，实现“数据可用不可见”。 产品平台涵盖了匿踪查询、隐私求交、联合建模、联合统计、算法容器管理、数据资源管理、数据确权与定价、异构平台互联互通等主要应用服务功能。
            </p>
          </dd>
        </dl>
        <dl>
          <dt>4.赛事日程</dt>
          <dd class="group-con text-center">
            <img src="../assets/images/process2.png" alt="">
          </dd>
        </dl>
        <dl>
          <dt>5.评比规则</dt>
          <dd>
            <p class="summary text-left">
            本次比赛分为初赛、复赛，初赛阶段只需要提交方案设计即可，具体形式不作限制，例如：文档、PPT、示例代码等待。通过专家评委的审核后，可以得到主办方提供的周边大礼包，并获得参加复赛的资格。
            </p>
            <p class="summary text-left">
              凡进入复赛的团队（个人），需要在指定时间内完成代码的编写、测试，确保功能完整、可用。参赛选手在线上提交，主办方组织专家评审，择优评出一二三等奖若干名。
            </p>
          </dd>
        </dl>
        <dl>
          <dt>6.奖项设置</dt>
          <dd>
            择优评出一等奖 1 组*3万，二等奖 2 组*2万，三等奖 3 组*1万，优胜奖-礼品证书。
          </dd>
        </dl>
      </div>
    </section>
    <section id="rules">
      <div class="section-title">
        <span class="title">参赛规则</span>
        <span class="background">Rules</span>
      </div>
      <div class="system-intro text-left">
        <dl>
          <dt>1.参赛材料</dt>
          <dd>
            <p>提交源码+路演PPT+5分钟以内演示视频。</p>
          </dd>
        </dl>
        <dl>
          <dt>2.提交方式</dt>
          <dd>
            参赛团队按报名审核提交材料清单，按要求准备材料后压缩成 zip 文档进行提交，提交网址为：（陆续开放）
          </dd>
        </dl>
        <dl>
          <dt>3.参赛回避</dt>
          <dd>
            <ul>
              <li>参赛者禁止在指定考核技术能力的范围外，利用规则漏洞或技术漏洞等不良途径提高成绩排名，禁止在比赛中交换答案或有其他违规行为，一经发现将取消比赛成绩并严肃处理。</li>
              <li>
                报名回避条件：
                <p>直接参与大赛策划、组织、技术服务提供、评审的工作人员、专家及其直系亲属不得作为参赛人员报名参加大赛。</p>
                <p>失信企业以及含有征信不良的参赛人员的团队不得报名参加大赛。</p>
              </li>
            </ul>
          </dd>
        </dl>
      </div>
    </section>
    </b-container>
    <b-container>
      <section id="experts">
        <div class="section-title">
          <span class="title">评审专家</span>
          <span class="background">Evaluation Experts</span>
        </div>
        <p style="font-size: 18px; margin-top: 10px"> - 排名不分先后 -</p>
        <div class="experts-container">
          <div class="experts" v-for="(item,index) in experts" :key="index" @mousemove="handleHover(index)" @mouseout="handleLeave">
            <div class="experts-avatar">
              <img :src="item.img" :alt="item.name">
            </div>
            <div class="experts-info text-left" :class="{'show': index === hoverIndex}">
              <p class="experts-name">{{item.name}}</p>
              <p v-html="item.desHtml"></p>
            </div>
          </div>
        </div>
      </section>
      <section id="contact" class="box text-left">
        <h3>联系我们</h3>
        <b-row class="mb-5 align-item-center">
          <b-col cols="6" sm="6"  class="qrcode">
            <p>大赛联系人：大赛小助手 <br>
              邮箱：<a target="blank" href="mailto:hackathon@openmpc.com">hackathon@openmpc.com</a>
            </p>
            <img src="../assets/images/hackathon.jpg" alt="郑超">
          </b-col>
          <b-col cols="6" sm="6" class="qrcode" >
            <p>大赛联系人：可隐<br>
              邮箱：<a target="blank" href="mailto:ppc@openmpc.com">ppc@openmpc.com</a>
            </p>
            <img src="../assets/images/xiongting.png" alt="可隐">
          </b-col>
        </b-row>
      </section>
      <div class="box text-left">
        <h3>权责说明</h3> 
        <p>主办方和承办方保留对比赛规则进行调整修改的权利，并对赛事相关事宜拥有最终解释权；<br>
        主办方和承办方保留调整赛事时间安排的权利，主办方和承办方有权在特殊条件下暂停或终止比赛；<br>
        主办方和承办方保留调整比赛各阶段入选团队数量的权利，以及调整奖项设置的权利；<br>
        对影响比赛组织及比赛公平性的参赛团队，主办方和承办方保留收回或拒绝授予其奖项的权利；<br>
        若因故出现数据更新、评审代码更新、作弊检查等原因主办方和承办方有权对参赛结果进行重新测评并更新排行榜。
        </p>
      </div>
      <hr>
      <section class="partners2" id="partners">
        <div class="section-title">
            <span class="title">合作伙伴</span>
            <span class="background">Partners</span>
          </div>
        <div class="partners-item">
          <h3>主办单位</h3>
          <div class="img-container">
            <img src="../assets/images/partner-logo-csdn.png" alt="CSDN">
            <img src="../assets/images/openmpc-logo.png" alt="openmpc logo">
          </div>
        </div>
        <div class="partners-item">
          <h3>合作伙伴</h3>
          <div class="img-container">
            <img src="../assets/images/partner-logo-datawhale.png" alt="Datawhale 开源社区">
            <img src="../assets/images/partner-logo-nebula-graph.png" alt="Nebula Graph">
            <img src="../assets/images/partner-logo-juicefs.png" alt="JuiceFS">
            <img src="../assets/images/partner-logo-mulan.png" alt="木兰开源社区">
            <img src="../assets/images/partner-logo-kubesphere.svg" alt="木兰开源社区">
            <img src="../assets/images/partner-logo-kaiyuanshe.png" alt="开源社">
            <img src="../assets/images/partner-logo-cskefu.png" alt="春松客服开源社区">
            <img src="../assets/images/partner-logo-osf2f.png" alt="开源面对面">
            <img src="../assets/images/partner-logo-iotdb.png" alt="IoTDB">
            <img src="../assets/images/partner-logo-jina.png" alt="Jina AI">
            <img src="../assets/images/partner-logo-wasm-edge-runtime.png" alt="Wasm Edge Runtime">
            <div v-if="!isExpired" class="placeholder-img">欢迎加入</div>  
          </div>
        </div>
        <div class="partners-item">
          <h3>赞助商</h3>
          <div class="img-container">
            <img src="../assets/images/partner-logo-primihub.png" alt="北京原语科技有限公司">
          </div>
        </div>
      </section>
    </b-container>
    <div class="text-center footer">
      <p>OpenMPC 版权所有 <a href="https://beian.miit.gov.cn/#/Integrated/index">冀ICP备16008495号-2</a></p>
    </div>
  <ToolTip text="报名即将开始" v-if="showTip" :visible="showTip"/>
  </div>
</template>

<script>
import NavBar from '@/components/NavBar.vue';
import ToolTip from '@/components/ToolTip.vue';

export default {
  name: 'PcIndex',
  components:{
    NavBar,
    ToolTip
  },
  data() {
    return {
      showTip: false,
      hoverIndex: -1,
      experts:[{
        name: '胡春明',
        img: require('../assets/images/guest-huchunming.jpg'),
        desHtml: '北京航空航天大学软件学院院长，教授，博导<br/>大数据科学与脑机智能北京市高精尖创新中心副主任。中国计算机学会系统软件专委会常务委员，计算机科学普及工委主任，中国电子学会云计算、大数据专家委员会副秘书长，国际万维网联盟（W3C）副理事长。'
      }, {
        name: '丁勇',
        img: require('../assets/images/guest-dingyong.jpg'),
        desHtml: '教授，博导，广西密码学与信息安全重点实验室主任、<br/>福建“闽江学者讲座教授”，教育部网络空间安全教指委委员、中国密码学会高级会员，中国计算机学会区块链专委会常务委员。'
      }, {
        name: '袁征',
        img: require('../assets/images/guest-yuanzheng.png'),
        desHtml: '北京电子科技学院教授<br/>中国科技大学和北京邮电大学网空安全专业兼职博导，北京邮电大学理学院密码学博士，清华大学（杨振宁）高等研究院数学博士后，美国clemson大学访问学者。国务院政府特殊津贴获得者。长江学者奖励计划”、中组部“青年拨尖人才” 、“国家自然科学基金”等项目的通信评审专家。研究方向：经典密码的设计、安全性分析及其应用。'
      }, {
        name: '张志勇',
        img: require('../assets/images/guest-zhangzhiyong.jpg'),
        desHtml: '河南科技大学信息工程学院副院长，河南省特聘教授，博导<br/>现任河南科技大学网络空间安全应用河南省国际联合实验室主任、信息工程学院副院长。'
      }, {
        name: '周苏岳',
        img: require('../assets/images/guest-zhousuyue.jpg'),
        desHtml: '元宇宙产业生态架构师，影视数字化技术专家<br/>现任马栏山（长沙）视频文创产业园管委会首席专家、长沙马栏山投资开发建设有限公司首席技术官，国家广电总局 5G 高新视频多场景应用重点实验室专家委员，湖南省百人计划人才，长沙市 C 类高层次人才。'
      }, {
        name: '郭华',
        img: require('../assets/images/guest-guohua.jpeg'),
        desHtml: '北京航空航天大学博导，副教授<br/>研究方向隐私计算，密码学，安全协议。主持国家自然科学基金、北京市自然科学基金、CCF-腾讯犀牛鸟基金课题、CCF-蚂蚁金服隐私保护基金课题等多项。指导学生获全国密码技术竞赛特等奖、一等奖全国大学生信息安全大赛一等奖等多项。'
      }, {
        name: '孙溢',
        img: require('../assets/images/guest-sunyi.jpg'),
        desHtml: '北京邮电大学副教授<br/>主要研究方向包括密码学、安全多方计算、区块链、大数据安全等。主持和参与多项国家863 、国家重点研发计划、国家自然科学基金青年基金、GF科研项目以及多项省部级科技项目和企业横向项目。'
      }, {
        name: '李博',
        img: require('../assets/images/guest-libo.jpeg'),
        desHtml: '北京航空航天大学计算机学院副研究员，博士<br/>北京市大数据科学与脑机智能高精尖中心高级研究员，爱丁堡大学访问学者，主要从事网络安全、工业互联网、大数据安全等方面的研究工作，已在TKDE、TNNLS、FGCS、《中国科学》等国内外重要期刊及IJCAI、VEE、LISA等高水平会议发表学术论文50余篇。'
      }, {
        name: '茹志强',
        img: require('../assets/images/guest-ruzhiqiang.jpg'),
        desHtml: '中国移动信息技术中心部门技术总监<br/>中国计算机学会 CCF 第十二届全国代表大会全国代表；他是 CDA首批技术合伙人；他是中国人工智能学会CAAI评审库专家 ；他是信用中国行业信用专家；他是中国信息通信研究院隐私计算联盟电信工作组联席组长。曾在阿里巴巴集团、大连万达集团、招商局集团控股子公司等多家单位任职，目前任中国移动通信集团信息技术中心（大数据中心）内设部门技术总监。'
      }, {
        name: '李萌',
        img: require('../assets/images/guest-limeng.jpeg'),
        desHtml: '合肥工业大学计算机与信息学院副研究员，博士，硕士生导师<br/>指导全国大学生信息安全竞赛作品赛并获国家二等奖3项、国家三等奖2项和省级二等奖1项；担任中国中文信息学会大数据安全与隐私保护专委会常务委员、CCF区块链专委会执委、CCF计算机应用专委会执委、CAAI智能信息网络专业委员会委员、CAAI青年工作委员会委员、中国计算机学会会员、中国密码学会会员、IEEE高级会员和多个学术期刊与会议的审稿人。'
      }, {
        name: '李增鹏',
        img: require('../assets/images/guest-lizengpeng.jpeg'),
        desHtml: '山东大学网络空间安全学院副研究员<br/>博/硕士研究生导师，入选“山东大学青年学者未来计划"。获哈尔滨工程大学（与美国弗吉尼亚联邦大学联合培养）工学博士学位。曾为新西兰奥克兰大学研究助理，英国兰卡斯特大学与新加坡科技与设计大学博士后研究员。主要研究兴趣为公钥密码协议与分布式安全计算，重点围绕后量子密码开展：格与同态加密，分布式与可验证计算，哈希证明与认证密钥交换。'
      }, {
        name: '赵晨',
        img: require('../assets/images/guest-zhaocheng.jpg'),
        desHtml: '璞跃中国管理合伙人<br/>他在中国带领专业团队致力于投资、加速本地科技创业项目成长，同时帮助海外顶尖科技创新企业进入中国并拓展国内市场。'
      }, {
        name: '李森',
        img: require('../assets/images/guest-liseng.jpg'),
        desHtml: '北航投资常务副总，星空投资管理合伙人<br/>围绕未来空天技术，医工交叉，下一代信息技术，新材料，新能源领域开展早期投资。投资案例:星河动力、e签宝、四象爱数、航宇智造、深光科技、吸力奇迹、全景声科技、未磁科技、微盟等项目。曾获中国技术创业协会“科技创业导师贡献奖”获得者，深圳市“鹏城新力”百花奖章。'
      }, {
        name: '范晶晶',
        img: require('../assets/images/fanjingjing2.jpg'),
        desHtml: '科鲸 CEO<br/>开源组织 Datawhale 发起人<br/>开源社成员'
      }, {
        name: '强锋',
        img: require('../assets/images/qiangfeng2.jpg'),
        desHtml: '微言科技 CTO<br/>曾任中国工商银行大数据与人工智能实验室资深数据科学家和联邦学习团队负责人'
      },{
        name: '胡春强',
        img: require('../assets/images/guest-huchuqiang.jpg'),
        desHtml: '重庆大学“百人计划”研究员、博导<br/>大数据与软件学院 数据科学系副系主任，教育部CPS重点实验室智能软件与服务计算研究所副所长'
      }],
      isExpired: false
    };
  },
  created(){
    this.isExpired = this.checkDate()
    console.log(this.isExpired);
  },
  methods:{
    signup() {
      if(!this.isExpired){
        this.showTips()
      } else {
        window.open('https://wenjuan.feishu.cn/m/cfm?t=sQi5inqsabHi-sf6b', '_blank').focus();
      }
    },
    showTips(){
      this.showTip = true
      setTimeout(() => {
        this.showTip = false
      }, 2000);
    },
    handleHover(index){
      this.hoverIndex = index
      console.log(index);
    },
    handleLeave(){
      this.hoverIndex = ''
    },
    checkDate() {
      const now = new Date().getTime()
      const end = new Date('2022/10/24 00:00:00').getTime()
      return end < now
    }
  }
}
</script>