<template>
  <div class="nav">
    <b-navbar type="dark" variant="dark" toggleable="lg" fixed="top" class="navbar-custom sticky sticky-dark" id="main-menu"  style="background-color: #4b2f9a;">
      <b-navbar-brand to="/">
          <img src="../assets/images/logo.png" alt="原语科技" class="logo"/>
        </b-navbar-brand>
        <b-navbar-toggle target="navbarCollapse"></b-navbar-toggle>

        <b-collapse id="navbarCollapse" is-nav>
          <b-navbar-nav id="navbar-navlist">
            <b-nav-item-dropdown text="新闻中心" class="dropdown">
              <b-dropdown-item href="https://live.csdn.net/list/zxff716 ">CSDN</b-dropdown-item>
              <b-dropdown-item href="https://live.csdn.net/room/zxff716/YCRz6iv6">Meetup</b-dropdown-item>
            </b-nav-item-dropdown>
            <b-nav-item :class="{active: activeIndex === index}" :href="`#${item.path}`" v-for="(item,index) in list" :key="index" :id="'navbar-links-'+ index" @click="handleClick(item.path,index)">{{item.name}}</b-nav-item>
          </b-navbar-nav>
        </b-collapse>
    </b-navbar>
    
  </div>
</template>

<script>
export default {
  name: 'NavBar',
  data() {
    return {
      activeIndex: 0,
      list: [
        {
          name: '大赛简介',
          path: 'intro'
        },
        {
          name: '活动赛制',
          path: 'system'
        },
        {
          name: '参赛规则',
          path: 'rules'
        },
        {
          name: '评审专家',
          path: 'experts'
        },
        {
          name: '联系我们',
          path: 'contact'
        },
      ],
    };
  },
  mounted() {
    // document.getElementById('navbar-links-0').className += 'active';
  },
  methods: {
    handleClick(path,index){
      const top = document.getElementById(path).offsetTop + 100
      this.activeIndex = index
      document.body.scrollTo({
        top: top,
        behavior: 'smooth'
      });
    },
    handleSelect(key,index) {
      this.activeIndex = index
      // this.$router.push({path: key})
    }
  }
}
</script>

<style lang="scss" scoped>
::v-deep .bg-dark{
  background-color: #4b2f9a!important;
}
::v-deep .nav-link{
  color: #fff;
}
::v-deep .dropdown-menu{
  background: rgba(66,39,143,.5);
}
::v-deep .dropdown-item{
  color: #fff;
  &:hover{
    background-color: #4b2f9a;
    color: #fff;
  }
}
.navbar{
  display: flex;
  padding: 1vh 5vw;
  justify-content: space-between;
}
.nav-item{
  height: 100%;
  margin: 0 10px;
  transition: all 0.5s linear;
  &.active{
    position: relative;
    &::after{
      content: '';
      position: absolute;
      bottom: -2px;
      left: 50%;
      transform: translate3d(-50%,0,0);
      width: 10vw;
      height: 2px;
      background: #fff;
      border-radius: 1px;
    }
  }
}

@media (min-width: 576px) { 
  .nav-item{
    &.active{
      &::after{
        width: 72%;
        height: 2px;
        bottom: -6px;
      }
    }
  }
}
.navbar-collapse{
  flex-grow: 0;
}
.logo{
  height: 5vh;
}
::v-deep .dropdown{
  a{
    padding-left: 0.8em;
  }
}
</style>