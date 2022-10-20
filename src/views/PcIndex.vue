<template>
  <div class="main">
    <NavBar />
    <div class="banner">
      <div class="container">
        <div class="button" @click="showTips"></div>
      </div>
    </div>
    <div class="container">
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
              <a class="nocolor" href="https://github.com/primihub/primihub" target="_blank">PrimiHub</a> 是基于安全多方计算、联邦学习、同态加密、可信计算等的隐私计算技术，结合区块链等研发的开源隐私计算应用平台。
                <a class="nocolor" href="https://github.com/primihub/primihub" target="_blank">PrimiHub</a> 秉承保护数据在应用过程中的隐私安全，实现“数据可用不可见”。 产品平台涵盖了匿踪查询、隐私求交、联合建模、联合统计、算法容器管理、数据资源管理、数据确权与定价、异构平台互联互通等主要应用服务功能。
              </p>
              <p class="summary text-left">
              本次比赛，邀请所有参赛团队基于 <a class="nocolor" href="https://github.com/primihub/primihub" target="_blank">PrimiHub</a> 技术及其服务进行开发，期待通过本场大赛，为隐私计算技术的推进和应用的发展提供新思路。
              </p>
            </dd>
          </dl>
          <dl>
            <dt>4.赛事日程</dt>
            <dd class="group-con text-center">
              <img src="../assets/images/process.png" alt="">
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
              择优评出一等奖两组*3万，二等奖4组*2万，三等奖六组*1万，优胜奖-礼品证书。
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

       <section id="experts">
        <div class="section-title">
          <span class="title">评审专家</span>
          <span class="background">Evaluation Experts</span>
        </div>
        <div class="experts-container">
         <div cols="5" sm="3" class="experts" v-for="(item,index) in experts" :key="index">
            <div class="experts-avatar" @mousemove="handleHover(index)" @mouseout="handleLeave">
              <img :src="item.img" :alt="item.name">
            </div>
            <div class="experts-info text-left" :class="{'show': index === hoverIndex}">
              <p class="experts-name">{{item.name}}</p>
              <p v-html="item.desHtml"></p>
            </div>
          </div>
        </div>
        <p>重量级评委待揭晓<br>……</p>
      </section>
      <section id="contact" class="box text-left">
        <h3>联系我们</h3>
        <b-row class="mb-5 align-item-center">
          <b-col cols="6" sm="6"  class="qrcode">
            <p>合作联系人：大赛小助手<br>
              邮箱：<a href="mailto:shackathon@openmpc.com" target="_blank">hackathon@openmpc.com</a>
            </p>
            <img src="../assets/images/qrcode-contact.png" alt="大赛小助手" width="198">
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
    </div>
    <section class="partners2">
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
          <div v-if="!expired" class="placeholder-img">欢迎加入</div>  
        </div>
      </div>
      <div class="partners-item">
        <h3>赞助商</h3>
        <div class="img-container">
          <img src="../assets/images/partner-logo-primihub.png" alt="北京原语科技有限公司">
        </div>
      </div>
    </section>
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
      experts:[
        {
          name: '范晶晶',
          img: require('../assets/images/fanjingjing2.png'),
          desHtml: '科鲸CEO<br>开源组织Datawhale发起人<br>开源社成员'
        },
        {
          name: '强锋',
          img: require('../assets/images/qiangfeng2.png'),
          desHtml: '微言科技CTO<br>曾任中国工商银行大数据与人工智能实验室资深数据科学家和联邦学习团队负责人'
        }
      ]
    };
  },
  computed:{
    expired(){
      const now = new Date().getTime()
      const end = new Date('2022-10-24 23:59:59').getTime()
      console.log((end - now) / 86400000+'天');
      if(end-now < 0){
        return true
      }else{
        return false
      }
    }
  },
  methods:{
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
    }
  }
}
</script>