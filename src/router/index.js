import Vue from 'vue'
import VueRouter from 'vue-router'
import HomeView from '../views/PcIndex.vue'

Vue.use(VueRouter)

// 解决重复点击路由报错的BUG
const originalPush = VueRouter.prototype.push
VueRouter.prototype.push = function push(location) {
  return originalPush.call(this, location).catch((err) => err)
}

const routes = [
  {
    path: '/',
    name: 'home',
    component: HomeView
  },
  { path: '*', redirect: '/', hidden: true }
]

const router = new VueRouter({
  routes
})

export default router
